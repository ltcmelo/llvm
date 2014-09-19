//===-- Y86InstrInfo.h - Y86 Instruction Information ------------*- C++ -*-===//
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

#ifndef Y86INSTRUCTIONINFO_H
#define Y86INSTRUCTIONINFO_H

#include "Y86RegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "Y86GenInstrInfo.inc"


namespace llvm {

class Y86RegisterInfo;
class Y86Subtarget;

class Y86InstrInfo final : public Y86GenInstrInfo {
public:
  explicit Y86InstrInfo(Y86Subtarget &ST);

  unsigned isLoadFromStackSlot(const MachineInstr *MI,
                               int &FrameIndex) const override;

  unsigned isStoreToStackSlot(const MachineInstr *MI,
                              int &FrameIndex) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI,
                            unsigned DestReg, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI,
                           unsigned SrcReg, bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  void copyPhysReg(MachineBasicBlock &MBB,
                   MachineBasicBlock::iterator I, DebugLoc DL,
                   unsigned DestReg, unsigned SrcReg,
                   bool KillSrc) const override;

  const Y86RegisterInfo &getRegisterInfo() const { return RI; }

private:
  virtual void anchor();

  const Y86RegisterInfo RI;
};

} // End llvm namespace

#endif
