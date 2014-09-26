//===-- X86AsmPrinter.h - X86 implementation of AsmPrinter ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef Y86ASMPRINTER_H
#define Y86ASMPRINTER_H

#include "Y86Subtarget.h"
#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {

class LLVM_LIBRARY_VISIBILITY Y86AsmPrinter : public AsmPrinter {
public:
  explicit Y86AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
    : AsmPrinter(TM, Streamer)
    , Subtarget(&TM.getSubtarget<Y86Subtarget>()) {}

  void EmitStartOfAsmFile(Module &M) override;
  void EmitEndOfAsmFile(Module &M) override;

  void EmitInstruction(const MachineInstr *MI) override;

  void EmitRegSetup(unsigned Reg);
  void EmitStartup();
  void EmitTermination();

  const char *getPassName() const override {
    return "Y86 Assembly Printer";
  }

private:
  const Y86Subtarget *Subtarget;
};

} // End namespace llvm

#endif
