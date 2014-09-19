//===-- Y86MCTargetDesc.cpp - Y86 Target Descriptions ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Y86 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Y86MCTargetDesc.h"
#include "Y86MCAsmInfo.h"
#include "InstPrinter/Y86MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "Y86GenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "Y86GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Y86GenSubtargetInfo.inc"


static MCAsmInfo *createY86MCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
  return new Y86MCAsmInfo(TT);
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Ctx, MCAsmBackend &MAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    const MCSubtargetInfo &STI,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

static MCInstPrinter *createY86MCInstPrinter(const Target &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI,
                                             const MCSubtargetInfo &STI) {
  return new Y86MCInstPrinter(MAI, MII, MRI);
}

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn A(TheY86Target, createY86MCAsmInfo);

  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheY86Target,
                                           createMCStreamer);
  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheY86Target,
                                        createY86MCInstPrinter);
}
