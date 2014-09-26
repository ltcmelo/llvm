//===-- Y86MCTargetStreamer.cpp - Y86 Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Y86MCTargetStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

Y86MCTargetStreamer::Y86MCTargetStreamer(MCStreamer &S)
  : MCTargetStreamer(S) {}

Y86MCAsmTargetStreamer::Y86MCAsmTargetStreamer(MCStreamer &S,
                                               formatted_raw_ostream &ROS)
  : Y86MCTargetStreamer(S)
  , OS(ROS) {}

void Y86MCAsmTargetStreamer::emitDirectivePos(unsigned Address) {
  OS << "\t.pos " << Address << "\n";
}
