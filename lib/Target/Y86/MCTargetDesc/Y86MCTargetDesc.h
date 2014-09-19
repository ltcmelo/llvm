//===-- Y86MCTargetDesc.h - Y86 Target Descriptions -------------*- C++ -*-===//
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

#ifndef Y86MCTARGETDESC_H
#define Y86MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {

class Target;

extern Target TheY86Target;

} // End llvm namespace


#define GET_REGINFO_ENUM
#include "Y86GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "Y86GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "Y86GenSubtargetInfo.inc"

#endif
