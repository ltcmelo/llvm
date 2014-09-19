//===-- Y86.h - Top-level interface for Y86 representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Y86 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef Y86_H
#define Y86_H

namespace llvm {

class FunctionPass;
class Y86TargetMachine;

FunctionPass *createY86ISelDag(Y86TargetMachine &TM);

} // End namespace llvm.

#endif
