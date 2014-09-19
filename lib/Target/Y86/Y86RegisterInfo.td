//===- Y86RegisterInfo.td - Describe the Y86 Register File --*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Y86 Register file, defining the registers themselves
// and the register classes built out of the registers.
//
//===----------------------------------------------------------------------===//

class Y86Reg<string n, bits<16> Enc> : Register<n> {
  let Namespace = "Y86";
  let HWEncoding = Enc;
}

//===----------------------------------------------------------------------===//
//  Register definitions...
//===----------------------------------------------------------------------===//

// Generic 32-bit registers
def EAX : Y86Reg<"eax", 0>;
def ECX : Y86Reg<"ecx", 1>;
def EDX : Y86Reg<"edx", 2>;
def EBX : Y86Reg<"ebx", 3>;
def ESP : Y86Reg<"esp", 4>;
def EBP : Y86Reg<"ebp", 5>;
def ESI : Y86Reg<"esi", 6>;
def EDI : Y86Reg<"edi", 7>;

// Instruction pointer
def EIP : Y86Reg<"eip", 0>;

// Status flags register
def EFLAGS : Y86Reg<"flags", 0>;

//===----------------------------------------------------------------------===//
// Register Class Definitions...
//===----------------------------------------------------------------------===//

// The order specified in the register list is implicitly defined to be the
// register allocation order.

def GPR : RegisterClass<"Y86", [i32], 32,
                        (add EAX, ECX, EDX, ESI, EDI, EBX, EBP, ESP)>;
