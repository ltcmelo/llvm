//===-- Y86MCInstPrinter.h - Convert Y86 MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Y86 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "Y86MCInstPrinter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "y86-asm-printer"

#define PRINT_ALIAS_INSTR
#include "Y86GenAsmWriter.inc"


void Y86MCInstPrinter::printRegName(raw_ostream &OS,
                                    unsigned RegNo) const{
  OS << '%' << StringRef(getRegisterName(RegNo)).lower();
}

void Y86MCInstPrinter::printInst(const MCInst *MI,
                                 raw_ostream &O,
                                 StringRef Annot) {
  printInstruction(MI, O);

  // Always print the annotation next.
  printAnnotation(O, Annot);
}

void Y86MCInstPrinter::printOperand(const MCInst *MI, int OpNo, raw_ostream &OS) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(OS, Op.getReg());
  } else if (Op.isImm()) {
    OS << '$' << formatImm((int64_t)Op.getImm());
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    OS << '$' << *Op.getExpr();
  }
}

void Y86MCInstPrinter::printMem(const MCInst *MI,
                                unsigned OpNo,
                                raw_ostream &OS) {
  OS << formatImm(MI->getOperand(OpNo + 1).getImm());
  OS << "(";
  printRegName(OS, MI->getOperand(OpNo).getReg());
  OS << ")";
}

void Y86MCInstPrinter::printPCRelImm(const MCInst *MI,
                                     unsigned OpNo,
                                     raw_ostream &OS) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm()) {
    OS << formatImm(Op.getImm());
  } else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    // If a symbolic branch target was added as a constant expression then print
    // that address in hex. Otherwise, just print the expression.
    int64_t Address;
    const MCConstantExpr *BranchTarget = dyn_cast<MCConstantExpr>(Op.getExpr());
    if (BranchTarget && BranchTarget->EvaluateAsAbsolute(Address)) {
      OS << formatHex((uint64_t)Address);
    } else {
      OS << *Op.getExpr();
    }
  }
}
