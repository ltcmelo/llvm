//===-- Y86ISelDAGToDAG.cpp - A dag to dag inst selector for Y86 ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Y86 target.
//
//===----------------------------------------------------------------------===//

#include "Y86TargetMachine.h"
#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "y86-isel"

namespace {

class Y86DAGToDAGISel : public SelectionDAGISel {
public:
  explicit Y86DAGToDAGISel(Y86TargetMachine &TM)
      : SelectionDAGISel(TM)
      , Subtarget(TM.getSubtarget<Y86Subtarget>()) {
  }

  SDNode *Select(SDNode *N) override;

  const char *getPassName() const override {
    return "Y86 DAG->DAG Pattern Instruction Selection";
  }

  // Include the pieces autogenerated from the target description.
#include "Y86GenDAGISel.inc"

private:
  const Y86Subtarget &Subtarget;

  bool SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset);
};

} // End anonymous namespace


SDNode *Y86DAGToDAGISel::Select(SDNode *Node) {
  if (Node->isMachineOpcode()) {
    Node->setNodeId(-1);
    return nullptr; // Already selected.
  }

  return SelectCode(Node);
}

/// SelectAddr - Return true if it is able to match the address operand
/// through a complex pattern check. It will assign a corresponding base
/// and offset for the access, which can either be a register or a
/// memory location.
bool Y86DAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset) {
  switch (Addr.getOpcode()) {
  default:
    llvm_unreachable("Y86DAGToDAGISel::SelectAddr default case");
    break;

  case ISD::CopyFromReg: {
    // There must be a direct connection from the load/store node, without
    // any displacement. We simply assign the register.
    Base = Addr;
    Offset = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
  }

  case ISD::ADD: {
    // An ADD node could be created during call lowering when pushing
    // arguments onto the stack. This is typically done by chaining a
    // CopyFromReg node as the first operand and a Constant as the second.
    Base = Addr.getOperand(0);
    ConstantSDNode *CN = cast<ConstantSDNode>(Addr.getOperand(1));
    Offset = CurDAG->getTargetConstant(CN->getSExtValue(), MVT::i32);
    return true;
  }

  case ISD::FrameIndex: {
    // A mapping from the abstract frame indices consists of retrieving
    // the exact index and making a pointer to it.
    FrameIndexSDNode *FIN = cast<FrameIndexSDNode>(Addr);
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(),
                                       getTargetLowering()->getPointerTy());
    Offset = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
  }
  }
}

//===----------------------------------------------------------------------===//

/// createY86ISelDag - This pass converts a legalized DAG into a
/// Y86-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createY86ISelDag(Y86TargetMachine &TM) {
  return new Y86DAGToDAGISel(TM);
}
