//===-- X86MCInstLower.cpp - Convert X86 MachineInstr to an MCInst --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower X86 MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "Y86AsmPrinter.h"
#include "Y86RegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "y86-inst-lower"

namespace {

class Y86MCInstLower {
public:
  Y86MCInstLower(const MachineFunction &MF, Y86AsmPrinter &Printer);

  void Lower(const MachineInstr *MI, MCInst &OutMI) const;
  MCSymbol *GetSymbolFromOperand(const MachineOperand &MO) const;
  MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

private:
  MCContext &Ctx;
  const TargetMachine &TM;
  const MCAsmInfo &MAI;
  Y86AsmPrinter &AsmPrinter;
};

} // End anonymous namespace


Y86MCInstLower::Y86MCInstLower(const MachineFunction &Function,
                               Y86AsmPrinter &Printer)
  : Ctx(Function.getContext())
  , TM(Function.getTarget())
  , MAI(*TM.getMCAsmInfo())
  , AsmPrinter(Printer) {
}

void Y86MCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType()) {
      default:
        MI->dump();
        llvm_unreachable("unknown operand type");
      case MachineOperand::MO_Register:
        // Ignore all implicit register operands.
        if (MO.isImplicit())
          continue;
        MCOp = MCOperand::CreateReg(MO.getReg());
        break;
      case MachineOperand::MO_Immediate:
        MCOp = MCOperand::CreateImm(MO.getImm());
        break;
      case MachineOperand::MO_MachineBasicBlock:
      case MachineOperand::MO_GlobalAddress:
      case MachineOperand::MO_ExternalSymbol:
        MCOp = LowerSymbolOperand(MO, GetSymbolFromOperand(MO));
        break;
      case MachineOperand::MO_JumpTableIndex:
        llvm_unreachable("Lower JumpTableIndex");
        /*MCOp = LowerSymbolOperand(MO, AsmPrinter.GetJTISymbol(MO.getIndex()));
        break;*/
      case MachineOperand::MO_ConstantPoolIndex:
        llvm_unreachable("Lower ConstantPoolIndex");
        /*MCOp = LowerSymbolOperand(MO, AsmPrinter.GetCPISymbol(MO.getIndex()));
        break;*/
      case MachineOperand::MO_BlockAddress:
        llvm_unreachable("Lower BLockAddress");
        /*MCOp = LowerSymbolOperand(MO,
                                  AsmPrinter.GetBlockAddressSymbol(MO.getBlockAddress()));*/
        break;
      case MachineOperand::MO_RegisterMask:
        // Ignore call clobbers.
        continue;
    }

    OutMI.addOperand(MCOp);
  }
}

MCSymbol *Y86MCInstLower::GetSymbolFromOperand(const MachineOperand &MO) const {
  assert((MO.isGlobal()
          || MO.isSymbol()
          || MO.isMBB())
         && "Isn't a symbol reference");

  SmallString<128> Name;
  if (MO.isGlobal()) {
    const GlobalValue *GV = MO.getGlobal();
    AsmPrinter.getNameWithPrefix(Name, GV);
  } else if (MO.isSymbol()) {
    AsmPrinter.Mang->getNameWithPrefix(Name, MO.getSymbolName());
  } else if (MO.isMBB()) {
    Name += MO.getMBB()->getSymbol()->getName();
  }

  return Ctx.GetOrCreateSymbol(Name);
}

MCOperand Y86MCInstLower::LowerSymbolOperand(const MachineOperand &MO,
                                             MCSymbol *Sym) const {
  MCSymbolRefExpr::VariantKind RefKind = MCSymbolRefExpr::VK_None;
  const MCExpr *Expr = MCSymbolRefExpr::Create(Sym, RefKind, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    Expr = MCBinaryExpr::CreateAdd(Expr,
                                   MCConstantExpr::Create(MO.getOffset(), Ctx),
                                   Ctx);

  return MCOperand::CreateExpr(Expr);
}

//===----------------------------------------------------------------------===//

void Y86AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  Y86MCInstLower MCInstLowering(*MF, *this);
  MCInst Inst;
  MCInstLowering.Lower(MI, Inst);
  EmitToStreamer(OutStreamer, Inst);
}
