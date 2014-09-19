//===-- Y86MachineFuctionInfo.cpp - Y86 machine function info -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Y86MachineFunctionInfo.h"

using namespace llvm;

Y86MachineFunctionInfo::Y86MachineFunctionInfo()
    : CalleeSavedFrameSize(0) {
}

Y86MachineFunctionInfo::Y86MachineFunctionInfo(MachineFunction &MF)
    : CalleeSavedFrameSize(0) {
}

void Y86MachineFunctionInfo::anchor() {}
