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
  Data8bitsDirective = "\t.byte\t";
  Data16bitsDirective = "\t.word\t";
  Data32bitsDirective = "\t.long\t";
  Data64bitsDirective = nullptr;

  HasDotTypeDotSizeDirective = false;
  HasSingleParameterDotFile = false;

  // TODO: We should have a HasGlobalDirective as well.
  GlobalDirective = "# [Global Symbol] ";
}

void Y86MCAsmInfo::anchor() {}
