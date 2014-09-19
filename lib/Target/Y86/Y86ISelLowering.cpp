//===-- Y86ISelLowering.cpp - Y86 DAG Lowering Implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that Y86 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Y86ISelLowering.h"
#include "Y86TargetMachine.h"
#include "Y86TargetObjectFile.h"
#include "MCTargetDesc/Y86MCTargetDesc.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "y86-isel"

#include "Y86GenCallingConv.inc"


Y86TargetLowering::Y86TargetLowering(TargetMachine &TM)
    : TargetLowering(TM, new Y86TargetObjectFile) {
  resetOperationActions();
}

void Y86TargetLowering::resetOperationActions() {
  // Set up the register classes.
  addRegisterClass(MVT::i32, &Y86::GPRRegClass);

  computeRegisterProperties();
}

SDValue Y86TargetLowering::
LowerFormalArguments(SDValue Chain,
                     CallingConv::ID CallConv,
                     bool isVarArg,
                     const SmallVectorImpl<ISD::InputArg> &Ins,
                     SDLoc dl,
                     SelectionDAG &DAG,
                     SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();

  // Gather info about the formals.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Y86);

  // Push corresponding SDValues for the arguments.
  SDValue ArgValue;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC;
      if (RegVT == MVT::i32)
        RC = &Y86::GPRRegClass;
      else
        llvm_unreachable("Unknown EVT");

      unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
      ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
    } else {
      assert(VA.isMemLoc());

      EVT ValVT;
      if (VA.getLocInfo() == CCValAssign::Indirect)
        ValVT = VA.getLocVT();
      else
        ValVT = VA.getValVT();

      // Create space in the stack (and an associated frame index)
      // accordingly: Either the complete size of the object is it's
      // a call by value or a pointer size otherwise.
      MachineFrameInfo *MFI = MF.getFrameInfo();
      ISD::ArgFlagsTy Flags = Ins[i].Flags;
      if (Flags.isByVal()) {
        unsigned Bytes = Flags.getByValSize();
        if (Bytes == 0)
          Bytes = 1; // Don't create zero-sized stack objects.
        int FI = MFI->CreateFixedObject(Bytes, VA.getLocMemOffset(), false);
        ArgValue = DAG.getFrameIndex(FI, getPointerTy());
      } else {
        int FI = MFI->CreateFixedObject(ValVT.getSizeInBits()/8,
                                        VA.getLocMemOffset(), true);
        SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
        ArgValue = DAG.getLoad(ValVT, dl, Chain, FIN,
                               MachinePointerInfo::getFixedStack(FI),
                               false, false, false, 0);
      }
    }

    InVals.push_back(ArgValue);
  }

  return Chain;
}

SDValue Y86TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  bool &isTailCall = CLI.IsTailCall;

  if (isTailCall) {
    // Here we would check whether we can actually eliminate the tail
    // call. But since we don't support it yet, we reject the opportunity
    // and also throw an error in the case there's an assumed expectation.
    if (CLI.CS && CLI.CS->isMustTailCall())
      report_fatal_error("tail call elimination not yet supported");
    isTailCall = false;
  }

  bool isVarArg = CLI.IsVarArg;
  SelectionDAG &DAG = CLI.DAG;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  CallingConv::ID CallConv = CLI.CallConv;
  MachineFunction &MFunction = DAG.getMachineFunction();

  if (isVarArg)
    report_fatal_error("varargs not yet supported");

  // Gather info about call operands.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MFunction, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Y86);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  // Start the calling sequence.
  SDValue Chain = CLI.Chain;
  SDLoc &dl = CLI.DL;
  Chain = DAG.getCALLSEQ_START(Chain,
                               DAG.getIntPtrConstant(NumBytes, true), dl);

  // Walk the register/memloc assignments and treat them for argument
  // passing (intermediate nodes maybe be used for chaining).
  const Y86RegisterInfo *RegInfo =
    static_cast<const Y86RegisterInfo*>(DAG.getSubtarget().getRegisterInfo());
  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    // Skip inalloca arguments, they have already been written.
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (Flags.isInAlloca())
      continue;

    SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[i];
    if (VA.isRegLoc()) {
      // Registers and corresponding arguments are added to a separate
      // list so they are chained all together later on.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());
      if (Flags.isByVal() || !isTailCall) {
        // First create a node to copy the arguments from a register (they
        // are accessed as a displacement from ESP).
        SDValue StackRegister =
            DAG.getCopyFromReg(Chain, dl, RegInfo->getStackRegister(),
                               getPointerTy());

        // The add node is used to adjust the offset to the exact location.
        unsigned MemLocOffset = VA.getLocMemOffset();
        SDValue MemLoc = DAG.getIntPtrConstant(MemLocOffset);
        MemLoc = DAG.getNode(ISD::ADD, dl, getPointerTy(),
                             StackRegister, MemLoc);

        // Complete the actual memory access.
        SDValue MemAccess;
        if (Flags.isByVal()) {
          SDValue SizeNode = DAG.getConstant(Flags.getByValSize(), MVT::i32);
          MemAccess = DAG.getMemcpy(Chain, dl, MemLoc, Arg, SizeNode,
                                    Flags.getByValAlign(), false, true,
                                    MachinePointerInfo(), MachinePointerInfo());
        } else {
          MemAccess = DAG.getStore(Chain, dl, Arg, MemLoc,
                                   MachinePointerInfo::getStack(MemLocOffset),
                                   false, false, 0);
        }
        MemOpChains.push_back(MemAccess);
      }
    }
  }

  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, MemOpChains);

  // Build a sequence of copy-to-reg nodes chained together with token chain
  // and flag operands which copy the outgoing args into registers.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl,
                             RegsToPass[i].first,   // Reg. assignment.
                             RegsToPass[i].second,  // Argument.
                             InFlag);
    InFlag = Chain.getValue(1);
  }

  SDValue Callee = CLI.Callee;
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    // If the callee is a GlobalAddress node (quite common, every direct call
    // is) turn it into a TargetGlobalAddress node so that legalize doesn't hack
    // it.
    const GlobalValue *GV = G->getGlobal();
    Callee = DAG.getTargetGlobalAddress(GV, dl, getPointerTy(),
                                        G->getOffset());
  } else {
    llvm_unreachable("External symbol calls not yet implemented");
  }

  // Fill-up call operands, beginning with the chain and callee.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const uint32_t *Mask = RegInfo->getCallPreservedMask(CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  SDVTList VTs = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(Y86ISD::CALL, dl, VTs, Ops);
  InFlag = Chain.getValue(1);

  // Finish the calling sequence. First operand is the total number of bytes
  // to pop. The second operand is the fraction of those bytes to be popped
  // by the callee.
  unsigned NumBytesForCalleeToPop = 0;
  if (Y86::isCalleePop(CallConv, isVarArg, isTailCall))
    NumBytesForCalleeToPop = NumBytes;
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getIntPtrConstant(NumBytes, true),
                             DAG.getIntPtrConstant(NumBytesForCalleeToPop, true),
                             InFlag, dl);
  InFlag = Chain.getValue(1);

  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg,
                         Ins, dl, DAG, InVals);

}

SDValue Y86TargetLowering::LowerCallResult(SDValue Chain,
                                           SDValue InFlag,
                                           CallingConv::ID CallConv,
                                           bool isVarArg,
                                           const SmallVectorImpl<ISD::InputArg> &Ins,
                                           SDLoc dl,
                                           SelectionDAG &DAG,
                                           SmallVectorImpl<SDValue> &InVals) const {
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());
  CCInfo.AnalyzeCallResult(Ins, RetCC_Y86);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0, e = RVLocs.size(); i != e; ++i) {
    CCValAssign &VA = RVLocs[i];
    EVT CopyVT = VA.getValVT();
    Chain = DAG.getCopyFromReg(Chain, dl, VA.getLocReg(),
                               CopyVT, InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

SDValue Y86TargetLowering::LowerReturn(SDValue Chain,
                                       CallingConv::ID CallConv,
                                       bool isVarArg,
                                       const SmallVectorImpl<ISD::OutputArg> &Outs,
                                       const SmallVectorImpl<SDValue> &OutVals,
                                       SDLoc dl,
                                       SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();

  // Gather info about the return values.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_Y86);

  SDValue Flag;
  SmallVector<SDValue, 6> RetOps(1, Chain); // Operand 0 is the chain.

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    SDValue ValToCopy = OutVals[i];
    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), ValToCopy, Flag);
    Flag = Chain.getValue(1); // Copies are glued together with flags.

    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update the chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(Y86ISD::RET_FLAG, dl, MVT::Other, RetOps);
}

const char *Y86TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
    default: return nullptr;
    case Y86ISD::CALL: return "Y86ISD::CALL";
    case Y86ISD::RET_FLAG: return "Y86ISD::RET_FLAG";
  }
}

//===----------------------------------------------------------------------===//

bool Y86::isCalleePop(CallingConv::ID CallingConv,
                      bool IsVarArg,
                      bool TailCallOpt) {
  // Currently only callee pop is supported.
  return true;
}
