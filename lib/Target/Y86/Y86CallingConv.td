//===-- Y86CallingConv.td - Calling Conventions Y86       --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Y86 architecture. They are
// simpflied versions of common X86 ones.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Convention
//===----------------------------------------------------------------------===//

def RetCC_Y86 : CallingConv<[
  // Values are returned in EAX first, then EDC and ECX.
  CCIfType<[i32], CCAssignToReg<[EAX, EDX, ECX]>>
]>;

//===----------------------------------------------------------------------===//
// Calling Convention
//===----------------------------------------------------------------------===//

def CC_Y86 :CallingConv<[
  // Handles byval parameters.
  CCIfByVal<CCPassByVal<4, 4>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR_NoRegs : CalleeSavedRegs<(add)>;

def CSR_32 : CalleeSavedRegs<(add ESI, EDI, EBX, EBP)>;
