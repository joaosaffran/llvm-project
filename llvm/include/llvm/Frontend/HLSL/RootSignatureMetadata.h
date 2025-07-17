//===- RootSignatureMetadata.h - HLSL Root Signature helpers --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file This file contains a library for working with HLSL Root Signatures and
/// their metadata representation.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_FRONTEND_HLSL_ROOTSIGNATUREMETADATA_H
#define LLVM_FRONTEND_HLSL_ROOTSIGNATUREMETADATA_H

#include "llvm/Frontend/HLSL/HLSLRootSignature.h"
#include "llvm/IR/Constants.h"
#include "llvm/MC/DXContainerRootSignature.h"

namespace llvm {
class LLVMContext;
class MDNode;
class Metadata;

namespace hlsl {
namespace rootsig {

inline std::optional<uint32_t> extractMdIntValue(MDNode *Node,
                                                 unsigned int OpId) {
  if (auto *CI =
          mdconst::dyn_extract<ConstantInt>(Node->getOperand(OpId).get()))
    return CI->getZExtValue();
  return std::nullopt;
}

inline std::optional<float> extractMdFloatValue(MDNode *Node,
                                                unsigned int OpId) {
  if (auto *CI = mdconst::dyn_extract<ConstantFP>(Node->getOperand(OpId).get()))
    return CI->getValueAPF().convertToFloat();
  return std::nullopt;
}

inline std::optional<StringRef> extractMdStringValue(MDNode *Node,
                                                     unsigned int OpId) {
  MDString *NodeText = dyn_cast<MDString>(Node->getOperand(OpId));
  if (NodeText == nullptr)
    return std::nullopt;
  return NodeText->getString();
}

template <typename T>
class RootSignatureValidationError
    : public ErrorInfo<RootSignatureValidationError<T>> {
public:
  static char ID;
  std::string ParamName;
  T Value;

  RootSignatureValidationError(StringRef ParamName, T Value)
      : ParamName(ParamName.str()), Value(Value) {}

  void log(raw_ostream &OS) const override {
    OS << "Invalid value for " << ParamName << ": " << Value;
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }
};

class GenericRSMetadataError : public ErrorInfo<GenericRSMetadataError> {
public:
  static char ID;
  std::string Message;

  GenericRSMetadataError(Twine Message) : Message(Message.str()) {}

  void log(raw_ostream &OS) const override { OS << Message; }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }
};

class InvalidRSMetadataFormat : public ErrorInfo<InvalidRSMetadataFormat> {
public:
  static char ID;
  std::string ElementName;

  InvalidRSMetadataFormat(StringRef ElementName)
      : ElementName(ElementName.str()) {}

  void log(raw_ostream &OS) const override {
    OS << "Invalid format for  " << ElementName;
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }
};

class InvalidRSMetadataValue : public ErrorInfo<InvalidRSMetadataValue> {
public:
  static char ID;
  std::string ParamName;

  InvalidRSMetadataValue(StringRef ParamName) : ParamName(ParamName.str()) {}

  void log(raw_ostream &OS) const override {
    OS << "Invalid value for " << ParamName;
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }
};

class MetadataBuilder {
public:
  MetadataBuilder(llvm::LLVMContext &Ctx, ArrayRef<RootElement> Elements)
      : Ctx(Ctx), Elements(Elements) {}

  /// Iterates through elements and dispatches onto the correct Build* method
  ///
  /// Accumulates the root signature and returns the Metadata node that is just
  /// a list of all the elements
  LLVM_ABI MDNode *BuildRootSignature();

private:
  /// Define the various builders for the different metadata types
  MDNode *BuildRootFlags(const dxbc::RootFlags &Flags);
  MDNode *BuildRootConstants(const RootConstants &Constants);
  MDNode *BuildRootDescriptor(const RootDescriptor &Descriptor);
  MDNode *BuildDescriptorTable(const DescriptorTable &Table);
  MDNode *BuildDescriptorTableClause(const DescriptorTableClause &Clause);
  MDNode *BuildStaticSampler(const StaticSampler &Sampler);

  llvm::LLVMContext &Ctx;
  ArrayRef<RootElement> Elements;
  SmallVector<Metadata *> GeneratedMetadata;
};

enum class RootSignatureElementKind {
  Error = 0,
  RootFlags = 1,
  RootConstants = 2,
  SRV = 3,
  UAV = 4,
  CBV = 5,
  DescriptorTable = 6,
  StaticSamplers = 7
};

class MetadataParser {
public:
  MetadataParser(MDNode *Root) : Root(Root) {}

  /// Iterates through root signature and converts them into MapT
  LLVM_ABI llvm::Expected<llvm::mcdxbc::RootSignatureDesc>
  ParseRootSignature(uint32_t Version);

private:
  llvm::Error parseRootFlags(mcdxbc::RootSignatureDesc &RSD,
                             MDNode *RootFlagNode);
  llvm::Error parseRootConstants(mcdxbc::RootSignatureDesc &RSD,
                                 MDNode *RootConstantNode);
  llvm::Error parseRootDescriptors(mcdxbc::RootSignatureDesc &RSD,
                                   MDNode *RootDescriptorNode,
                                   RootSignatureElementKind ElementKind);
  llvm::Error parseDescriptorRange(mcdxbc::DescriptorTable &Table,
                                   MDNode *RangeDescriptorNode);
  llvm::Error parseDescriptorTable(mcdxbc::RootSignatureDesc &RSD,
                                   MDNode *DescriptorTableNode);
  llvm::Error parseRootSignatureElement(mcdxbc::RootSignatureDesc &RSD,
                                        MDNode *Element);
  llvm::Error parseStaticSampler(mcdxbc::RootSignatureDesc &RSD,
                                 MDNode *StaticSamplerNode);

  llvm::Error validateRootSignature(const llvm::mcdxbc::RootSignatureDesc &RSD);

  MDNode *Root;
};

} // namespace rootsig
} // namespace hlsl
} // namespace llvm

#endif // LLVM_FRONTEND_HLSL_ROOTSIGNATUREMETADATA_H
