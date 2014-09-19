##===- lib/Target/Y86/Makefile -----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMY86CodeGen
TARGET = Y86

BUILT_SOURCES = Y86GenSubtargetInfo.inc \
                Y86GenRegisterInfo.inc  \
                Y86GenInstrInfo.inc \
                Y86GenDAGISel.inc \
                Y86GenAsmWriter.inc \
                Y86GenCallingConv.inc

# DIRS = AsmParser Disassembler Utils
DIRS = InstPrinter TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common
