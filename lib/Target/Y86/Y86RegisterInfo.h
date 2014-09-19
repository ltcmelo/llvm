//===-- Y86RegisterInfo.h - Y86 Register Information Impl -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Y86 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Y86REGISTERINFO_H
#define Y86REGISTERINFO_H

#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "Y86GenRegisterInfo.inc"

namespace llvm {

class Y86Subtarget;

class Y86RegisterInfo final : public Y86GenRegisterInfo {
public:
  Y86RegisterInfo(const Y86Subtarget &STI);

  const MCPhysReg* getCalleeSavedRegs(const MachineFunction* MF) const override;

  const uint32_t *getCallPreservedMask(CallingConv::ID) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                                unsigned Kind = 0) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator MI,
                           int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  unsigned getFrameRegister(const MachineFunction &MF) const override;
  unsigned getStackRegister() const { return StackPtr; }

private:
  unsigned StackPtr;
  unsigned FramePtr;
};

} // End llvm namespace

#endif
