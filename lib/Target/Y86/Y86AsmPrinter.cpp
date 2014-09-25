//===-- Y86AsmPrinter.h - Y86 implementation of AsmPrinter ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Y86AsmPrinter.h"
#include "MCTargetDesc/Y86MCTargetStreamer.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86AsmPrinter() {
  RegisterAsmPrinter<Y86AsmPrinter> X(TheY86Target);
}

Y86MCTargetStreamer &targetStreamer(MCStreamer &S) {
  return static_cast<Y86MCTargetStreamer &>(*S.getTargetStreamer());
}

void Y86AsmPrinter::EmitStartOfAsmFile(Module &M) {
  Y86MCTargetStreamer &TS = targetStreamer(OutStreamer);
  TS.emitDirectivePos(0);
}

void Y86AsmPrinter::EmitEndOfAsmFile(Module &M) {
  Y86MCTargetStreamer &TS = targetStreamer(OutStreamer);
  TS.emitDirectivePos(0x400);
  MCSymbol *StackSym = OutContext.GetOrCreateSymbol(StringRef("Stack"));
  OutStreamer.EmitLabel(StackSym);
}
