//===-- Y86AsmPrinter.h - Y86 implementation of AsmPrinter ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Y86AsmPrinter.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86AsmPrinter() {
  RegisterAsmPrinter<Y86AsmPrinter> X(TheY86Target);
}
