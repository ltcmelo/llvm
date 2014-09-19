//===-- Y86TargetFrameLowering.h - Define frame lowering for Y86 -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements Y86-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef Y86_FRAMELOWERING_H
#define Y86_FRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {

class Y86Subtarget;

class Y86FrameLowering : public TargetFrameLowering {
public:
  explicit Y86FrameLowering(const Y86Subtarget &ST);

  void emitPrologue(MachineFunction &MF) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool assignCalleeSavedSpillSlots(MachineFunction &MF,
                                   const TargetRegisterInfo *TRI,
                                   std::vector<CalleeSavedInfo> &CSI) const override;
  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const override;
  bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MI,
                                   const std::vector<CalleeSavedInfo> &CSI,
                                   const TargetRegisterInfo *TRI) const override;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MI) const override;

  bool hasFP(const MachineFunction &MF) const override;

  bool hasReservedCallFrame(const MachineFunction &MF) const override;

  int getFrameIndexOffset(const MachineFunction &MF, int FI) const override;

private:
  unsigned SlotSize;
};

} // End llvm namespace

#endif
