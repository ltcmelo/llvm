//===-- Y86InstrFormats.td - Y86 Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Y86 Instruction Format Definition.
//
// A non-official format naming convention.
//
//    T  - Trivial instructions with just an opcode (icode/ifun).
//    R  - Register only instructions, with an opcode and a register
//         specifier byte.
//    I  - Immediate instructions, with an opcode, a target register
//         specifier, and a constant value.
//    M  - Memory instructions, with an ppcode, a register specifier
//         byte, and a memory displacement.
//    J  - Jump-like instructions, with an opcode and a destination
//         memory address.
//
//===----------------------------------------------------------------------===//

// Format specifies the encoding used by the instruction. This is part of the
// ad-hoc solution used to emit machine instruction encodings by our machine
// code emitter.
class Format<bits<3> val> {
  bits<3> Value = val;
}

def Null : Format<0>;
def T : Format<1>;
def R : Format<2>;
def I : Format<3>;
def M : Format<4>;
def J : Format<5>;


class Y86Inst<bits<8> opcode,
              Format format,
              dag outs, dag ins,
              string asmstr,
              list<dag> pattern>
    : Instruction {
  dag OutOperandList = outs;
  dag InOperandList = ins;
  string AsmString = asmstr;
  let Pattern = pattern;

  let Namespace = "Y86";
  let isCodeGenOnly = !eq(!cast<string>(format), "Pseudo");
  bits<8> Opcode = opcode;
  Format Form = format;

  // This must be kept in sync with instruction definitions.
  let TSFlags{7-0} = Opcode;
  let TSFlags{10-8} = Form.Value;
}


// The following classes are an initial categorization of instructions
// by their functions. Details (or specializations) to be added later.

class ArithLogic<bits<8> opcode,
                 Format format,
                 dag outs, dag ins,
                 string asmstr,
                 list<dag> pattern>
    : Y86Inst<opcode, format, outs, ins, asmstr, pattern> {
}

class FlowControl<bits<8> opcode,
                  Format format,
                  dag outs, dag ins,
                  string asmstr,
                  list<dag> pattern>
    : Y86Inst<opcode, format, outs, ins, asmstr, pattern> {
}

class DataMove<bits<8> opcode,
               Format format,
               dag outs, dag ins,
               string asmstr,
               list<dag> pattern>
    : Y86Inst<opcode, format, outs, ins, asmstr, pattern> {
}

class StackManip<bits<8> opcode,
                 Format format,
                 dag outs, dag ins,
                 string asmstr,
                 list<dag> pattern>
    : Y86Inst<opcode, format, outs, ins, asmstr, pattern> {
}

class Pseudo<dag outs, dag ins,
             list<dag> pattern>
    : Y86Inst<0xFF, Null, outs, ins, "#pseudo", pattern> {
}
