//===- llvm/MC/DXContainerRootSignature.h - RootSignature -*- C++ -*- ========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/DXContainer.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include <utility>

namespace llvm {

class raw_ostream;
namespace mcdxbc {

union DescriptorTable {
  dxbc::DescriptorRangeV10 RangesV10;
  dxbc::DescriptorRangeV11 RangesV11;
};

struct RootParameter {
  dxbc::RootParameterType ParameterType;
  dxbc::ShaderVisibility ShaderVisibility;
  uint32_t Version;
  union {
    dxbc::RootConstants Constants;
    dxbc::RootDescriptorV10 DescriptorV10;
    dxbc::RootDescriptorV11 DescriptorV11;
    SmallVector<DescriptorTable> DescriptorTable;
  };

  RootParameter(uint32_t Version, dxbc::RootParameterType Type,
                dxbc::ShaderVisibility Visibility)
      : ParameterType(Type), ShaderVisibility(Visibility), Version(Version) {}

  // Destructor that properly cleans up the active union member
  ~RootParameter() {
    switch (ParameterType) {
    case dxbc::RootParameterType::Constants32Bit:
      Constants.~RootConstants();
      break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV:
      if (Version == 1)
        DescriptorV10.~RootDescriptorV10();
      if (Version == 2)
        DescriptorV11.~RootDescriptorV11();
      break;
    case llvm::dxbc::RootParameterType::DescriptorTable:
      DescriptorTable.~SmallVector();
      break;

    default:
      llvm_unreachable("Invalid Root parameter type");
    }
  }

  // Copy constructor
  RootParameter(const RootParameter &Other)
      : ParameterType(Other.ParameterType),
        ShaderVisibility(Other.ShaderVisibility), Version(Other.Version) {
    switch (ParameterType) {
    case dxbc::RootParameterType::Constants32Bit:
      Constants = dxbc::RootConstants(Other.Constants);
      break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV:
      if (Version == 1)
        DescriptorV10 = dxbc::RootDescriptorV10(Other.DescriptorV10);
      else if (Version == 2)
        DescriptorV11 = dxbc::RootDescriptorV11(Other.DescriptorV11);
      break;
    case llvm::dxbc::RootParameterType::DescriptorTable:
      DescriptorTable = Other.DescriptorTable;
      break;
    default:
      llvm_unreachable("Invalid Root parameter type");
    }
  }

  RootParameter &operator=(const RootParameter &Other) {
    if (this != &Other) {
      this->~RootParameter();
      new (this) RootParameter(Other);
    }
    return *this;
  }

  RootParameter(RootParameter &&Other) noexcept
      : ParameterType(std::move(Other.ParameterType)),
        ShaderVisibility(std::move(Other.ShaderVisibility)),
        Version(std::move(Other.Version)) {
    switch (ParameterType) {
    case dxbc::RootParameterType::Constants32Bit:
      Constants = dxbc::RootConstants(std::move(Other.Constants));
      break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV:
      if (Version == 1)
        DescriptorV10 = dxbc::RootDescriptorV10(std::move(Other.DescriptorV10));
      else if (Version == 2)
        DescriptorV11 = dxbc::RootDescriptorV11(std::move(Other.DescriptorV11));
      break;
    case llvm::dxbc::RootParameterType::DescriptorTable:
      DescriptorTable = std::move(Other.DescriptorTable);
      break;
    default:
      llvm_unreachable("Invalid Root parameter type");
    }
  }

  RootParameter &operator=(RootParameter &&Other) noexcept {
    if (this != &Other) {
      this->~RootParameter();
      new (this) RootParameter(std::move(Other));
    }
    return *this;
  }
};
struct RootSignatureDesc {

  dxbc::RootSignatureHeader Header;
  SmallVector<mcdxbc::RootParameter> Parameters;
  RootSignatureDesc()
      : Header(dxbc::RootSignatureHeader{
            2, 0, sizeof(dxbc::RootSignatureHeader), 0, 0, 0}) {}

  void write(raw_ostream &OS) const;

  size_t getSize() const {
    return sizeof(dxbc::RootSignatureHeader) + Parameters.size_in_bytes();
  }
};
} // namespace mcdxbc
} // namespace llvm
