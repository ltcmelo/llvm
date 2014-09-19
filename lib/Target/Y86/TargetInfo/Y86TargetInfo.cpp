//===-- Y86TargetInfo.cpp - Y86 Target Implementation ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheY86Target;

extern "C" void LLVMInitializeY86TargetInfo() {
  RegisterTarget<Triple::y86, false> X(TheY86Target,
                                       "y86",
                                       "Y86 (32-bit) architecture");
}
