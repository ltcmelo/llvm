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
#include "MCTargetDesc/Y86MCTargetStreamer.h"
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


static MCAsmInfo *createMCAsmInfo(const MCRegisterInfo &MRI,
                                  StringRef TT) {
  return new Y86MCAsmInfo(TT);
}

static MCStreamer *createMCAsmStreamer(MCContext &Ctx,
                                       formatted_raw_ostream &OS,
                                       bool isVerboseAsm,
                                       bool useDwarfDirectory,
                                       MCInstPrinter *InstPrint,
                                       MCCodeEmitter *CE,
                                       MCAsmBackend *TAB,
                                       bool ShowInst) {
  MCStreamer *S =
      llvm::createAsmStreamer(Ctx, OS, isVerboseAsm, useDwarfDirectory,
                              InstPrint, CE, TAB, ShowInst);
  new Y86MCAsmTargetStreamer(*S, OS);
  return S;
}

/*
static MCStreamer *createMCObjectStreamer(const Target &T, StringRef TT,
                                          MCContext &Ctx, MCAsmBackend &MAB,
                                          raw_ostream &_OS,
                                          MCCodeEmitter *_Emitter,
                                          const MCSubtargetInfo &STI,
                                          bool RelaxAll,
                                          bool NoExecStack) {
  return 0;
}
*/

static MCInstPrinter *createMCInstPrinter(const Target &T,
                                          unsigned SyntaxVariant,
                                          const MCAsmInfo &MAI,
                                          const MCInstrInfo &MII,
                                          const MCRegisterInfo &MRI,
                                          const MCSubtargetInfo &STI) {
  return new Y86MCInstPrinter(MAI, MII, MRI);
}

//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeY86TargetMC() {
  // Register general target asm information.
  RegisterMCAsmInfoFn A(TheY86Target, createMCAsmInfo);

  // Register the asm streamer.
  TargetRegistry::RegisterAsmStreamer(TheY86Target,
                                      createMCAsmStreamer);

  // Register the oject streamer.
  /*TargetRegistry::RegisterMCObjectStreamer(TheY86Target,
                                           createMCObjectStreamer);*/

  // Register the instruction printer.
  TargetRegistry::RegisterMCInstPrinter(TheY86Target,
                                        createMCInstPrinter);
}
