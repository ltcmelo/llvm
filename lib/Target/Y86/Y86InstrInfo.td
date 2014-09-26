//===-- Y86InstrInfo.td - Main Y86 Instruction Definition --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Y86 instruction set, defining the instructions, and
// properties of the instructions which are needed for code generation, machine
// code emission, and analysis.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Y86 Type Profiles
//===----------------------------------------------------------------------===//

def SDT_Y86CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_Y86CallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_Y86Call : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

//===----------------------------------------------------------------------===//
// Y86 DAG Nodes
//===----------------------------------------------------------------------===//

// These are target-independent nodes, but have target-specific formats.
def Y86callseq_start : SDNode<"ISD::CALLSEQ_START",
                              SDT_Y86CallSeqStart,
                              [SDNPHasChain, SDNPOutGlue]>;
def Y86callseq_end : SDNode<"ISD::CALLSEQ_END",
                            SDT_Y86CallSeqEnd,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// Call instructions are translated in LowerCall for DAG building.
def Y86call : SDNode<"Y86ISD::CALL",
                     SDT_Y86Call,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

// Ret instructions are translated in LowerReturn for DAG building.
def Y86retflag : SDNode<"Y86ISD::RET_FLAG",
                        SDTNone,
                        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//===----------------------------------------------------------------------===//
// Y86 Operand Classes
//===----------------------------------------------------------------------===//

def MemDispAsmOperandClass : AsmOperandClass {
  let Name = "MemDisp";
}

def AbsMemAsmOperandClass : AsmOperandClass {
  let Name = "AbsMem";
}

//===----------------------------------------------------------------------===//
// Y86 Operands
//===----------------------------------------------------------------------===//

class MemDispOperand<string printMethod> : Operand<iPTR> {
  let PrintMethod = printMethod;
  let MIOperandInfo = (ops ptr_rc, i32imm);
  let ParserMatchClass = MemDispAsmOperandClass;
}

let OperandType = "OPERAND_MEMORY" in {
def mem : MemDispOperand<"printMem">;
}

let OperandType = "OPERAND_PCREL",
    ParserMatchClass = AbsMemAsmOperandClass,
    PrintMethod = "printPCRelImm" in {
def immPCRel : Operand<i32>;
}

//===----------------------------------------------------------------------===//
// Y86 Complex Pattern Definitions
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], []>;

//===----------------------------------------------------------------------===//
// X86 Scheduling Definitions
//===----------------------------------------------------------------------===//

def WriteLoad : SchedWrite;
def WriteStore : SchedWrite;
def WriteMove : SchedWrite;
def WriteZero : SchedWrite;

//===----------------------------------------------------------------------===//
// X86 Instruction List
//===----------------------------------------------------------------------===//

include "Y86InstrFormats.td"

def HALT : FlowControl<0x00, T, (outs), (ins), "halt", []>;

let hasSideEffects = 0,
    SchedRW = [WriteZero] in {
def NOP : FlowControl<0x10, T, (outs), (ins), "nop", []>;
}

let hasSideEffects = 0,
    SchedRW = [WriteMove] in {
def RRMOVL : DataMove<0x20, R,
                      (outs GPR:$dst), (ins GPR:$src),
                      "rrmovl $src, $dst",
                      []>;
}

let isReMaterializable = 1,
    isAsCheapAsAMove = 1 in {
def IRMOVL : DataMove<0x30, I,
                      (outs GPR:$dst), (ins i32imm:$src),
                      "irmovl $src, $dst",
                      [(set GPR:$dst, imm:$src)]>;
}

let SchedRW = [WriteStore] in {
def RMMOVL : DataMove<0x40, M,
                      (outs), (ins mem:$dst, GPR:$src),
                      "rmmovl $src, $dst",
                      [(store GPR:$src, addr:$dst)]>;
}

let canFoldAsLoad = 1,
    isReMaterializable =1,
    SchedRW = [WriteLoad] in {
def MRMOVL : DataMove<0x50, M,
                      (outs GPR:$dst), (ins mem:$src),
                      "mrmovl $src, $dst",
                      [(set GPR:$dst, (load addr:$src))]>;
}

let isTerminator = 1,
    isReturn = 1,
    isBarrier = 1,
    hasCtrlDep = 1 in {
def RET : FlowControl<0x90, T,
                      (outs), (ins),
                      "ret",
                      [(Y86retflag)]>;
}

let isCall = 1,
    Uses = [ESP] in {
def CALL : FlowControl<0x80, J,
                       (outs), (ins immPCRel:$dst),
                       "call $dst",
                       [(Y86call (i32 tglobaladdr:$dst))]>;
}

let mayStore = 1,
    SchedRW = [WriteStore] in {
def PUSHL : StackManip<0xA0, R,
                       (outs), (ins GPR:$reg),
                       "pushl $reg",
                       []>;
}

let Defs = [ESP],
    Uses = [ESP],
    hasSideEffects = 0,
    mayLoad = 1,
    SchedRW = [WriteLoad] in {
def POPL : StackManip<0xB0, R,
                      (outs GPR:$reg), (ins),
                      "popl $reg",
                      []>;
}

def ADDL : ArithLogic<0x60, R,
                      (outs GPR:$dst), (ins GPR:$src1, GPR:$src2),
                      "addl $src1, $src2",
                      []>;

def SUBL : ArithLogic<0x61, R,
                      (outs GPR:$dst), (ins GPR:$src1, GPR:$src2),
                      "subl $src1, $src2",
                      []>;

// ADJCALLSTACKDOWN/UP implicitly use/def RSP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber EFLAGS.
let Defs = [ESP, EFLAGS],
    Uses = [ESP] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),
                              [(Y86callseq_start timm:$amt)]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            [(Y86callseq_end timm:$amt1, timm:$amt2)]>;
}


//===----------------------------------------------------------------------===//
// Y86 Standalone Pattern Matching
//===----------------------------------------------------------------------===//

def : Pat<(add GPR:$src1, GPR:$src2), (ADDL GPR:$src1, GPR:$src2)>;
def : Pat<(sub GPR:$src1, GPR:$src2), (SUBL GPR:$src1, GPR:$src2)>;
