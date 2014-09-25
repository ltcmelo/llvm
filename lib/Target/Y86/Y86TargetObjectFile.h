//===-- llvm/Target/Y86TargetObjectFile.h - Y86 Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef Y86TARGETOBJECTFILE_H
#define Y86TARGETOBJECTFILE_H

#include "MCTargetDesc/Y86MCSection.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class Y86TargetObjectFile : public TargetLoweringObjectFile {

public:
  Y86TargetObjectFile() {}

  ~Y86TargetObjectFile() {
    delete TextSection;
    delete DataSection;
    delete BSSSection;
    delete ReadOnlySection;
  }

  void Initialize(MCContext &ctx, const TargetMachine &TM) override {
    TargetLoweringObjectFile::Initialize(ctx, TM);
    TextSection = new Y86MCSection(SectionKind::getText());
    DataSection = new Y86MCSection(SectionKind::getDataRel());
    BSSSection = new Y86MCSection(SectionKind::getBSS());
    ReadOnlySection = new Y86MCSection(SectionKind::getReadOnly());
  }

  const MCSection *getSectionForConstant(SectionKind Kind,
                                         const Constant *C) const override {
    return ReadOnlySection;
  }

  const MCSection *getExplicitSectionGlobal(const GlobalValue *GV,
                                            SectionKind Kind, Mangler &Mang,
                                            const TargetMachine &TM) const override {
    return DataSection;
  }
};

} // End namespace llvm

#endif
