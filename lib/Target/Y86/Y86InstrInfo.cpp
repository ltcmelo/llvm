//===-- Y86InstrInfo.cpp - Y86 Instruction Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Y86 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Y86InstrInfo.h"
#include "Y86Subtarget.h"
#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "y86-instr-info"

#define GET_INSTRINFO_CTOR_DTOR
#include "Y86GenInstrInfo.inc"


Y86InstrInfo::Y86InstrInfo(Y86Subtarget &STI)
    : Y86GenInstrInfo(Y86::ADJCALLSTACKDOWN, Y86::ADJCALLSTACKUP)
    , RI(STI) {
}

unsigned Y86InstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                           int &FrameIndex) const {
  // A direct load happens when the register is assigned from a frame index
  // operand with no displacement (the immediate is 0).
  if (MI->getOpcode() == Y86::MRMOVL) {
    if (MI->getOperand(1).isFI()
        && MI->getOperand(2).isImm()
        && MI->getOperand(2).getImm() == 0) {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

unsigned Y86InstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                          int &FrameIndex) const {
  // A direct store happens when the register assigns to a frame index
  // operand with no displacement (the immediate is 0).
  if (MI->getOpcode() == Y86::RMMOVL) {
    if (MI->getOperand(0).isFI()
        && MI->getOperand(1).isImm()
        && MI->getOperand(1).getImm() == 0) {
      FrameIndex = MI->getOperand(0).getIndex();
      return MI->getOperand(2).getReg();
    }
  }
  return 0;
}

void Y86InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I,
                                        unsigned DestReg, int FrameIndex,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI) const {
  llvm_unreachable("loadRegFromStackSlot not yet implemented");
}

void Y86InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I,
                                       unsigned SrcReg, bool isKill,
                                       int FrameIndex,
                                       const TargetRegisterClass *RC,
                                       const TargetRegisterInfo *TRI) const {
  llvm_unreachable("storeRegToStackSlot not yet implemented");
}

void Y86InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MBBI,
                               DebugLoc DL,
                               unsigned DestReg,
                               unsigned SrcReg,
                               bool KillSrc) const {
  if (Y86::GPRRegClass.contains(DestReg, SrcReg)) {
    unsigned Opcode = Y86::RRMOVL;
    BuildMI(MBB, MBBI, DL, get(Opcode), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }

  if (SrcReg == Y86::EFLAGS || DestReg == Y86::EFLAGS)
      llvm_unreachable("EFLAGS register copy not implemented");

  llvm_unreachable("Cannot emit physreg copy instruction");
}

void Y86InstrInfo::anchor() {}
