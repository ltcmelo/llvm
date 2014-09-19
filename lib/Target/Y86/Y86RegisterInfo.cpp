//===-- Y86RegisterInfo.cpp - Y86 Register Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Y86 implementation of the TargetRegisterInfo class.
// This file is responsible for the frame pointer elimination optimization
// on Y86.
//
//===----------------------------------------------------------------------===//

#include "Y86RegisterInfo.h"
#include "Y86Subtarget.h"
#include "Y86TargetMachine.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

#define DEBUG_TYPE "y86-register-info"

#define GET_REGINFO_TARGET_DESC
#include "Y86GenRegisterInfo.inc"


Y86RegisterInfo::Y86RegisterInfo(const Y86Subtarget &STI)
    : Y86GenRegisterInfo(Y86::EIP)
    , StackPtr(Y86::ESP)
    , FramePtr(Y86::EBP) {
}

const MCPhysReg *Y86RegisterInfo::
getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_32_SaveList;
}

const uint32_t *Y86RegisterInfo::getCallPreservedMask(CallingConv::ID) const {
  return CSR_32_RegMask;
}

BitVector Y86RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(Y86::ESP);
  Reserved.set(Y86::EIP);
  Reserved.set(Y86::EBP);
  return Reserved;
}

const TargetRegisterClass *Y86RegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const {
  return &Y86::GPRRegClass;
}

unsigned Y86RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Y86::EBP;
}

void Y86RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj,
                                          unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();

  // This is a memory reference with displacement. We always use EBP as the
  // basis for the address calculation. Therefore we consider its offset (in
  // regards to the frame index) and then calculate a new immediate from this
  // this value - "adjusting" the original.
  int64_t Offset = TFI->getFrameIndexOffset(MF, FrameIndex) +
                   MI.getOperand(FIOperandNum + 1).getImm();
  MI.getOperand(FIOperandNum).ChangeToRegister(FramePtr, false);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
}
