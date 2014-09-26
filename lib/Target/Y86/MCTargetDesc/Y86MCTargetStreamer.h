//===-- Y86MCTargetStreamer.h - Y86 Target Streamer ------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef Y86MCTARGETSTREAMER_H
#define Y86MCTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class Y86MCTargetStreamer : public MCTargetStreamer {
public:
  Y86MCTargetStreamer(MCStreamer &S);

  virtual void emitDirectivePos(unsigned Address) = 0;
};

class Y86MCAsmTargetStreamer : public Y86MCTargetStreamer {
public:
  Y86MCAsmTargetStreamer(MCStreamer &S, formatted_raw_ostream &OS);

  void emitDirectivePos(unsigned Address) override;

private:
  formatted_raw_ostream &OS;
};

} // End namespace llvm

#endif
