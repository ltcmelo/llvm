//===-- llvm/Target/Y86/MCTargetDesc/Y86MCSection - Object File Section   -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef Y86MCSECTION_H
#define Y86MCSECTION_H

#include "llvm/MC/MCSection.h"

namespace llvm {

/// \brief The Y86MCSection class
///
/// A trivial section class to be used in Y86 (.ys) assembly files.
///
class Y86MCSection : public MCSection {
public:
  Y86MCSection(SectionKind K)
    : MCSection(MCSection::SV_ELF /*Irrelevant*/, K) {}

  ~Y86MCSection() {}

  // We must override this to make sure nothing is printed.
  void PrintSwitchToSection(const MCAsmInfo &MAI,
                            raw_ostream &OS,
                            const MCExpr *Subsection) const override {}

  bool UseCodeAlign() const override { return false; }
  bool isVirtualSection() const override { return false; }
  std::string getLabelBeginName() const override { return ""; }
  std::string getLabelEndName() const override { return ""; }
};

} // End namespace llvm

#endif
