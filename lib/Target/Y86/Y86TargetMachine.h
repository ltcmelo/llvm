//===-- Y86TargetMachine.h - Define TargetMachine for the Y86 ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Y86 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef Y86TARGETMACHINE_H
#define Y86TARGETMACHINE_H

#include "Y86InstrInfo.h"
#include "Y86Subtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class StringRef;

class Y86TargetMachine final : public LLVMTargetMachine {
public:
  Y86TargetMachine(const Target &Target,
                   StringRef TargetTriple,
                   StringRef CPU,
                   StringRef FS,
                   const TargetOptions &TargetOptions,
                   Reloc::Model RM,
                   CodeModel::Model CM,
                   CodeGenOpt::Level OL);

  const Y86Subtarget *getSubtargetImpl() const override {
    return &Subtarget;
  }

  // Set up the pass pipeline.
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

private:
  virtual void anchor();
  Y86Subtarget Subtarget;
};

} // End llvm namespace

#endif
