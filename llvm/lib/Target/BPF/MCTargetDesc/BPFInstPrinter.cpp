//===-- BPFInstPrinter.cpp - Convert BPF MCInst to asm syntax -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an BPF MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/BPFInstPrinter.h"
#include "BPF.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#include "BPFGenAsmWriter.inc"

void BPFInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void BPFInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    O << formatImm((int32_t)Op.getImm());
  } else {
    assert(Op.isExpr() && "Expected an expression");
    MAI.printExpr(O, *Op.getExpr());
  }
}

void BPFInstPrinter::printMemOperand(const MCInst *MI, int OpNo,
                                     raw_ostream &O) {
  const MCOperand &RegOp = MI->getOperand(OpNo);
  const MCOperand &OffsetOp = MI->getOperand(OpNo + 1);

  // register
  assert(RegOp.isReg() && "Register operand not a register");
  O << getRegisterName(RegOp.getReg());

  // offset
  if (OffsetOp.isImm()) {
    auto Imm = OffsetOp.getImm();
    if (Imm >= 0)
      O << " + " << formatImm(Imm);
    else
      O << " - " << formatImm(-Imm);
  } else {
    assert(0 && "Expected an immediate");
  }
}

void BPFInstPrinter::printImm64Operand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << formatImm(Op.getImm());
  else if (Op.isExpr())
    MAI.printExpr(O, *Op.getExpr());
  else
    O << Op;
}

void BPFInstPrinter::printBrTargetOperand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm()) {
    if (MI->getOpcode() == BPF::JMPL) {
      int32_t Imm = Op.getImm();
      O << ((Imm >= 0) ? "+" : "") << formatImm(Imm);
    } else {
      int16_t Imm = Op.getImm();
      O << ((Imm >= 0) ? "+" : "") << formatImm(Imm);
    }
  } else if (Op.isExpr()) {
    MAI.printExpr(O, *Op.getExpr());
  } else {
    O << Op;
  }
}
