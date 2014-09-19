//===-- Y86MCAsmInfo.h - Y86 asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Y86MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Y86MCASMINFO_H
#define Y86MCASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {

class StringRef;

class Y86MCAsmInfo : public MCAsmInfo {
public:
  explicit Y86MCAsmInfo(StringRef TT);

private:
  virtual void anchor();
};

} // End llvm namespace

#endif
