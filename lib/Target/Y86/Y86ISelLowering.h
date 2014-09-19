//===-- Y86ISelLowering.h - Y86 DAG Lowering Interface ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Y86 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef Y86ISELLOWERING_H
#define Y86ISELLOWERING_H

#include "Y86.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/IR/CallingConv.h"

namespace llvm {

namespace Y86ISD {

  // Y86 Specific DAG Nodes
  enum NodeType {
    // Start the numbering from where ISD NodeType finishes.
    FIRST_NUMBER = ISD::BUILTIN_OP_END,

    /// Function call.
    CALL,

    /// Return with a flag.
    RET_FLAG
  };

} // End namespace Y86ISD

namespace Y86 {

  /// isCalleePop - Determines whether the callee is required to pop its
  /// own arguments. Callee pop is necessary to support tail calls.
  bool isCalleePop(CallingConv::ID CallingConv, bool IsVarArg, bool TailCallOpt);

} // End namespace Y86


class TargetMachine;

class Y86TargetLowering final : public TargetLowering {
public:
  explicit Y86TargetLowering(TargetMachine &TM);

  SDValue LowerFormalArguments(SDValue Chain,
                               CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               SDLoc dl,
                               SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCallResult(SDValue Chain,
                          SDValue InFlag,
                          CallingConv::ID CallConv,
                          bool isVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins,
                          SDLoc dl,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;

  SDValue LowerReturn(SDValue Chain,
                      CallingConv::ID CallConv,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals,
                      SDLoc dl,
                      SelectionDAG &DAG) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;

private:
  void resetOperationActions() override;
};

} // End namespace llvm

#endif
