//===-- Y86MachineFuctionInfo.h - Y86 machine function info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Y86-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef Y86MACHINEFUNCTIONINFO_H
#define Y86MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class Y86MachineFunctionInfo : public MachineFunctionInfo {
public:
  Y86MachineFunctionInfo();
  Y86MachineFunctionInfo(MachineFunction &MF);

  unsigned getCalleeSavedFrameSize() const { return CalleeSavedFrameSize; }
  void setCalleeSavedFrameSize(unsigned bytes) { CalleeSavedFrameSize = bytes; }

private:
  virtual void anchor();

  /// CalleeSavedFrameSize - Size of the callee-saved register portion of the
  /// stack frame in bytes.
  unsigned CalleeSavedFrameSize;
};

} // End namespace llvm

#endif
