//===- llvm/MC/DXContainerRootSignature.h - RootSignature -*- C++ -*- ========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/DXContainer.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class raw_ostream;
namespace mcdxbc {

struct RootParameter {
  dxbc::RootParameterHeader Header;
  uint32_t Version;
  union {
    dxbc::RootConstants Constants;
    dxbc::RootDescriptorV10 DescriptorV10;
    dxbc::RootDescriptorV11 DescriptorV11;
  };

  RootParameter(uint32_t Version, dxbc::RootParameterType Type,
                dxbc::ShaderVisibility Visibility)
      : Header(dxbc::RootParameterHeader{Type, Visibility, 0}),
        Version(Version) {}

  // Destructor that properly cleans up the active union member
  ~RootParameter() {
    switch (Header.ParameterType) {
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
    default:
      llvm_unreachable("Invalid Root parameter type");
    }
  }

  // Copy constructor
  RootParameter(const RootParameter &Other)
      : Header(Other.Header), Version(Other.Version) {
    switch (Header.ParameterType) {
    case dxbc::RootParameterType::Constants32Bit:
      new (&Constants) dxbc::RootConstants(Other.Constants);
      break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV:
      if (Version == 1)
        new (&DescriptorV10) dxbc::RootDescriptorV10(Other.DescriptorV10);
      else if (Version == 2)
        new (&DescriptorV11) dxbc::RootDescriptorV11(Other.DescriptorV11);
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
      : Header(std::move(Other.Header)), Version(std::move(Other.Version)) {
    switch (Header.ParameterType) {
    case dxbc::RootParameterType::Constants32Bit:
      new (&Constants) dxbc::RootConstants(std::move(Other.Constants));
      break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV:
      if (Version == 1)
        new (&DescriptorV10)
            dxbc::RootDescriptorV10(std::move(Other.DescriptorV10));
      else if (Version == 2)
        new (&DescriptorV11)
            dxbc::RootDescriptorV11(std::move(Other.DescriptorV11));
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
