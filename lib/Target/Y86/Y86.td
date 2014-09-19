//===-- Y86.td - Target definition file for the Intel Y86 --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a target description file for the Y86 architecture.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Y86 Subtarget state
//===----------------------------------------------------------------------===//

def Mode32Bit : SubtargetFeature<"32bit-mode", "In32BitMode", "true",
                                 "Standard (32-bit) mode">;

//===----------------------------------------------------------------------===//
// Y86 Subtarget features
//===----------------------------------------------------------------------===//

// Variations...

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "Y86RegisterInfo.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Y86InstrInfo.td"

def Y86InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Calling Conventions
//===----------------------------------------------------------------------===//

include "Y86CallingConv.td"

//===----------------------------------------------------------------------===//
// Assembly Printers
//===----------------------------------------------------------------------===//

def BasicAsmWriter : AsmWriter {
  string AsmWriterClassName = "MCInstPrinter";
}

def Y86 : Target {
  // Information about the instructions...
  let InstructionSet = Y86InstrInfo;
  let AssemblyWriters = [BasicAsmWriter];
}
