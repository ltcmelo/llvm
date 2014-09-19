//===-- Y86TargetMachine.cpp - Define TargetMachine for the Y86 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Y86 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "Y86TargetMachine.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

namespace {

class Y86PassConfig : public TargetPassConfig {
public:
  Y86PassConfig(Y86TargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  Y86TargetMachine &getY86TargetMachine() const {
    return getTM<Y86TargetMachine>();
  }

  const Y86Subtarget &getY86Subtarget() const {
    return *getY86TargetMachine().getSubtargetImpl();
  }

  bool addInstSelector() override;
};

} // End anonymous namespace

bool Y86PassConfig::addInstSelector() {
  // Install the Y86 instruction selector.
  addPass(createY86ISelDag(getY86TargetMachine()));
  return false;
}

//===----------------------------------------------------------------------===//

Y86TargetMachine::Y86TargetMachine(const Target &Target,
                                   StringRef TargetTriple,
                                   StringRef CPU,
                                   StringRef FS,
                                   const TargetOptions &Options,
                                   Reloc::Model RM,
                                   CodeModel::Model CM,
                                   CodeGenOpt::Level OL)
    : LLVMTargetMachine(Target, TargetTriple, CPU, FS, Options, RM, CM, OL)
    , Subtarget(TargetTriple, CPU, FS, *this) {
  initAsmInfo();
}

void Y86TargetMachine::anchor() {}

TargetPassConfig *Y86TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Y86PassConfig(this, PM);
}

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86Target() {
  RegisterTargetMachine<Y86TargetMachine> X(TheY86Target);
}
