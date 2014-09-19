//===-- Y86MCAsmInfo.cpp - Y86 asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the Y86MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "Y86MCAsmInfo.h"

using namespace llvm;

Y86MCAsmInfo::Y86MCAsmInfo(StringRef TT) {
  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";
  Data64bitsDirective = nullptr;

  // Directive ORG -> POS ?
}

void Y86MCAsmInfo::anchor() {}
