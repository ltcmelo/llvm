//===-- Y86FrameLowering.cpp - Y86 Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Y86 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Y86FrameLowering.h"
#include "Y86RegisterInfo.h"
#include "Y86Subtarget.h"
#include "Y86MachineFunctionInfo.h"
#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "y86-frame-lowering"

Y86FrameLowering::Y86FrameLowering(const Y86Subtarget &ST)
    : TargetFrameLowering(StackGrowsDown, 4, ST.is32Bit() ? -4 : -8)
    , SlotSize(ST.is32Bit() ? 4 : 8) {
}

/// findDeadCallerSavedReg - Return a caller-saved register that isn't live
/// when it reaches the "return" instruction. We can then pop a stack object
/// to this register without worry about clobbering it.
static unsigned findDeadCallerSavedReg(MachineBasicBlock::iterator &MBBI,
                                       const TargetRegisterInfo &TRI) {
  static const uint16_t CallerSavedRegs[] = {
    Y86::EAX, Y86::EDX, Y86::ECX, 0
  };

  if (MBBI->getOpcode() == Y86::RET) {
    SmallSet<uint16_t, 8> Uses;
    for (unsigned i = 0, e = MBBI->getNumOperands(); i != e; ++i) {
      MachineOperand &MO = MBBI->getOperand(i);
      if (!MO.isReg() || MO.isDef())
        continue;
      unsigned Reg = MO.getReg();
      if (!Reg)
        continue;
      for (MCRegAliasIterator AI(Reg, &TRI, true); AI.isValid(); ++AI)
        Uses.insert(*AI);
    }

    const uint16_t *CS = CallerSavedRegs;
    for (; *CS; ++CS)
      if (!Uses.count(*CS))
        return *CS;
  }

  return 0;
}

/// updateESP - Emit a series of instructions to increment/decrement
/// the stack pointer.
static void updateESP(MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator &MBBI,
                      int64_t NumBytes,
                      const TargetInstrInfo &TII,
                      const TargetRegisterInfo &TRI) {
  bool isSub = NumBytes < 0;
  unsigned Opcode = isSub ? Y86::SUBL : Y86::ADDL;

  DebugLoc DL = MBB.findDebugLoc(MBBI);
  uint64_t Offset = isSub ? -NumBytes : NumBytes;
  uint64_t Chunk = (1LL << 31) - 1; // Split large offsets into chunks.
  while (Offset) {
    unsigned AvailableReg = isSub
        ? static_cast<unsigned>(Y86::EAX)
        : findDeadCallerSavedReg(MBBI, TRI);

    if (!AvailableReg)
      report_fatal_error("Cannot allocate auxilary register");

    uint64_t ThisVal = (Offset > Chunk) ? Chunk : Offset;
    if (ThisVal == 4) {
      // If it's only a byte, use push/pop instead.
      Opcode = isSub ? Y86::PUSHL : Y86::POPL;
      MachineInstr *MI = BuildMI(MBB, MBBI, DL, TII.get(Opcode))
          .addReg(AvailableReg,
                  getDefRegState(!isSub) | getUndefRegState(isSub));
      if (isSub)
        MI->setFlag(MachineInstr::FrameSetup);
      Offset -= ThisVal;
      continue;
    }

    // Y86 integer operation instructions only operate on registers,
    // so we handle this in two steps.
    MachineInstr *MI1 = BuildMI(MBB, MBBI, DL, TII.get(Y86::IRMOVL), AvailableReg)
        .addImm(ThisVal);
    MachineInstr *MI2 = BuildMI(MBB, MBBI, DL, TII.get(Opcode), Y86::ESP)
        .addReg(AvailableReg)
        .addReg(Y86::ESP);
    //MI2->getOperand(3).setIsDead(); // The EFLAGS implicit def is dead.

    if (isSub) {
      MI1->setFlag(MachineInstr::FrameSetup);
      MI2->setFlag(MachineInstr::FrameSetup);
    }
    Offset -= ThisVal;
  }
}

void Y86FrameLowering::emitPrologue(MachineFunction &MF) const {
  MachineBasicBlock &MBB = MF.front(); // Prologue goes in entry BB.
  MachineBasicBlock::iterator MBBI = MBB.begin();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  DebugLoc DL;

  MachineFrameInfo *FrameInfo = MF.getFrameInfo();
  uint64_t StackSize = FrameInfo->getStackSize();
  Y86MachineFunctionInfo *FunctionInfo = MF.getInfo<Y86MachineFunctionInfo>();

  uint64_t NumBytes = 0; // Number of bytes to adjust ESP with.

  if (hasFP(MF)) {
    // Push EBP on top of stack to save it.
    BuildMI(MBB, MBBI, DL, TII.get(Y86::PUSHL))
        .addReg(Y86::EBP, RegState::Kill)
        .setMIFlag(MachineInstr::FrameSetup);

    // Update EBP with current ESP, the new base value.
    BuildMI(MBB, MBBI, DL, TII.get(Y86::RRMOVL), Y86::EBP)
        .addReg(Y86::ESP)
        .setMIFlag(MachineInstr::FrameSetup);

    // Mark EBP as live for every BB in the function.
    for (MachineFunction::iterator MFIt = MF.begin(), E = MF.end(); MFIt != E; ++MFIt)
      MFIt->addLiveIn(Y86::EBP);

    uint64_t FrameSize = StackSize - SlotSize; // Account for EBP in stack.
    NumBytes = FrameSize - FunctionInfo->getCalleeSavedFrameSize();
  } else {
    NumBytes = StackSize - FunctionInfo->getCalleeSavedFrameSize();
  }

  // Perform ESP update (increment/decrement).
  if (NumBytes)
    updateESP(MBB, MBBI, -NumBytes, TII, *MF.getSubtarget().getRegisterInfo());
}

void Y86FrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  DebugLoc DL;

  MachineFrameInfo *FrameInfo = MF.getFrameInfo();
  uint64_t StackSize = FrameInfo->getStackSize();
  Y86MachineFunctionInfo *FunctionInfo = MF.getInfo<Y86MachineFunctionInfo>();

  uint64_t NumBytes = 0; // Number of bytes to adjust back ESP with.

  if (hasFP(MF)) {
    uint64_t FrameSize = StackSize - SlotSize; // EBP will be popped later.
    NumBytes = FrameSize - FunctionInfo->getCalleeSavedFrameSize();
  } else {
    NumBytes = StackSize - FunctionInfo->getCalleeSavedFrameSize();
  }

  // Perform ESP update (increment/decrement).
  if (NumBytes)
    updateESP(MBB, MBBI, NumBytes, TII, *MF.getSubtarget().getRegisterInfo());

  if (hasFP(MF)) {
    // Restore EBP.
    BuildMI(MBB, MBBI, DL, TII.get(Y86::POPL), Y86::EBP);
  }
}

bool Y86FrameLowering::assignCalleeSavedSpillSlots(MachineFunction &MF,
                                                   const TargetRegisterInfo *TRI,
                                                   std::vector<CalleeSavedInfo> &CSI) const {
  // Spilling slots start from the local area offset.
  int SpillSlotOffset = getOffsetOfLocalArea();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  if (hasFP(MF)) {
    // Create a slot for EBP, which is always spilled by emitPrologue.
    SpillSlotOffset -=  SlotSize;
    MFI->CreateFixedSpillStackObject(SlotSize, SpillSlotOffset);

    // Make sure that EBP is not in the CSI list, since its spilling
    // and restoring are already handled by emitPrologue and emitEpilogue.
    const Y86RegisterInfo *RegInfo =
        static_cast<const Y86RegisterInfo *>(MF.getSubtarget().getRegisterInfo());
    unsigned FPReg = RegInfo->getFrameRegister(MF);
    for (unsigned i = 0; i < CSI.size(); ++i) {
      if (CSI[i].getReg() == FPReg) {
        CSI.erase(CSI.begin() + i);
        break;
      }
    }
  }

  // Assign slots for GPRs. It increases frame size.
  unsigned CalleeSavedFrameSize = 0;
  for (unsigned i = CSI.size(); i != 0; --i) {
    unsigned Reg = CSI[i - 1].getReg();
    if (!Y86::GPRRegClass.contains(Reg))
      continue;

    SpillSlotOffset -= SlotSize;
    int SlotIndex = MFI->CreateFixedSpillStackObject(SlotSize, SpillSlotOffset);
    CSI[i - 1].setFrameIdx(SlotIndex);

    CalleeSavedFrameSize += SlotSize;
  }

  Y86MachineFunctionInfo *FunctionInfo = MF.getInfo<Y86MachineFunctionInfo>();
  FunctionInfo->setCalleeSavedFrameSize(CalleeSavedFrameSize);

  return true;
}

bool Y86FrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                                 MachineBasicBlock::iterator MI,
                                                 const std::vector<CalleeSavedInfo> &CSI,
                                                 const TargetRegisterInfo *TRI) const {
  for (unsigned i = CSI.size(); i != 0; --i) {
    unsigned Reg = CSI[i - 1].getReg();
    if (!Y86::GPRRegClass.contains(Reg))
      continue;

    // Add the callee-saved register as live-in. It's killed at the spill.
    MBB.addLiveIn(Reg);

    // Emit the actual instructions to push the registers to the slots
    // assigned to them by assignCalleeSavedSpillSlots.
    MachineFunction &MF = *MBB.getParent();
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    DebugLoc DL = MBB.findDebugLoc(MI);
    BuildMI(MBB, MI, DL, TII.get(Y86::PUSHL)).addReg(Reg, RegState::Kill)
      .setMIFlag(MachineInstr::FrameSetup);
  }

  return true;
}

bool Y86FrameLowering::
restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI,
                            const std::vector<CalleeSavedInfo> &CSI,
                            const TargetRegisterInfo *TRI) const {
  if (CSI.empty())
    return false;

  // Pop the registers previously pushed onto the stack by
  // spillCalleeSavedRegisters.
  for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
    unsigned Reg = CSI[i].getReg();
    if (!Y86::GPRRegClass.contains(Reg))
      continue;

    MachineFunction &MF = *MBB.getParent();
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    DebugLoc DL = MBB.findDebugLoc(MI);
    BuildMI(MBB, MI, DL, TII.get(Y86::POPL), Reg);
  }
  return true;
}

void Y86FrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF,
                              MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const {
  if (!hasReservedCallFrame(MF)) {
    llvm_unreachable("Ops... So far call frame should alwyas be reserved.");
  }

  MBB.erase(MI);
}

bool Y86FrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF)
      || MFI->hasVarSizedObjects()
      || MFI->isFrameAddressTaken();
}

bool Y86FrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  // Call frame will be reserved unless there are variable size objects.
  return !MF.getFrameInfo()->hasVarSizedObjects();
}

int Y86FrameLowering::getFrameIndexOffset(const MachineFunction &MF,
                                          int FI) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  int Offset = MFI->getObjectOffset(FI) - getOffsetOfLocalArea();

  if (!hasFP(MF))
    return Offset + MFI->getStackSize();

  // Take in the saved EBP in consideration and skip one slot.
  return Offset + SlotSize;
}
