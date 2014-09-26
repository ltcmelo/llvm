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

Y86MCTargetStreamer &targetStreamer(MCStreamer &S) {
  return static_cast<Y86MCTargetStreamer &>(*S.getTargetStreamer());
}

void Y86AsmPrinter::EmitStartOfAsmFile(Module &M) {
  Y86MCTargetStreamer &TS = targetStreamer(OutStreamer);

  TS.emitDirectivePos(0);
  OutStreamer.EmitLabel(OutContext.GetOrCreateSymbol(StringRef("init")));
  EmitRegSetup(Y86::ESP);
  EmitRegSetup(Y86::EBP);
  EmitStartup();
  EmitTermination();
  OutStreamer.AddBlankLine();
}

void Y86AsmPrinter::EmitEndOfAsmFile(Module &M) {
  Y86MCTargetStreamer &TS = targetStreamer(OutStreamer);

  TS.emitDirectivePos(Subtarget->getInitialStackLocation());
  OutStreamer.EmitLabel(OutContext.GetOrCreateSymbol(StringRef("stack")));
}

void Y86AsmPrinter::EmitRegSetup(unsigned Reg) {
  MCInst Inst;
  Inst.setOpcode(Y86::IRMOVL);
  Inst.addOperand(MCOperand::CreateReg(Reg));
  MCSymbol *Stack = OutContext.GetOrCreateSymbol(StringRef("stack"));
  Inst.addOperand(MCOperand::CreateExpr(
                    MCSymbolRefExpr::Create(Stack, OutContext)));
  OutStreamer.EmitInstruction(Inst, getSubtargetInfo());
}

void Y86AsmPrinter::EmitStartup() {
  MCInst Inst;
  Inst.setOpcode(Y86::CALL);
  MCSymbol *Main = OutContext.GetOrCreateSymbol(StringRef("main"));
  Inst.addOperand(MCOperand::CreateExpr(
                    MCSymbolRefExpr::Create(Main, OutContext)));
  OutStreamer.EmitInstruction(Inst, getSubtargetInfo());
}

void Y86AsmPrinter::EmitTermination() {
  MCInst Inst;
  Inst.setOpcode(Y86::HALT);
  OutStreamer.EmitInstruction(Inst, getSubtargetInfo());
}

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86AsmPrinter() {
  RegisterAsmPrinter<Y86AsmPrinter> X(TheY86Target);
}
