//===-- Y86Subtarget.cpp - Y86 Subtarget Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Y86 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "Y86Subtarget.h"

using namespace llvm;

#define DEBUG_TYPE "y86-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Y86GenSubtargetInfo.inc"

namespace {

std::string computeDataLayout(const Y86Subtarget &ST) {
  // Little-endian.
  std::string Ret = "e";

  // Pointers...
  if (ST.is32Bit())
    Ret += "-p:32:32";
  else
    Ret += "-p:64:64";

  // 32-bit registers.
  Ret += "-n32";

  return Ret;
}

} // End anonymous namespace

Y86Subtarget::Y86Subtarget(const std::string &TT,
                           const std::string &CPU,
                           const std::string &FS,
                           TargetMachine &TM)
    : Y86GenSubtargetInfo(TT, CPU, FS)
    , In32BitMode(true)
    , InitialStackLocation(0x400)
    , DL(computeDataLayout(*this))
    , InstrInfo(*this)
    , FrameLowering(*this)
    , TargetLowering(TM) {
}
