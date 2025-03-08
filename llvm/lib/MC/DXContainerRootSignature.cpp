//===- llvm/MC/DXContainerRootSignature.cpp - RootSignature -*- C++ -*-=======//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/DXContainerRootSignature.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/BinaryFormat/DXContainer.h"
#include "llvm/Support/EndianStream.h"
#include <cstddef>
#include <cstdint>

using namespace llvm;
using namespace llvm::mcdxbc;

static uint32_t writePlaceholder(raw_svector_ostream &Stream) {
  const uint32_t DummyValue = std::numeric_limits<uint32_t>::max();
  uint32_t Offset = Stream.tell();
  support::endian::write(Stream, DummyValue, llvm::endianness::little);
  return Offset;
}

static void rewriteOffset(raw_svector_ostream &Stream, uint32_t Offset) {
  uint32_t Value =
      support::endian::byte_swap<uint32_t, llvm::endianness::little>(
          Stream.tell());
  Stream.pwrite(reinterpret_cast<const char *>(&Value), sizeof(Value), Offset);
}

void RootSignatureDesc::write(raw_ostream &OS) const {
  SmallString<256> Storage;
  raw_svector_ostream BOS(Storage);
  BOS.reserveExtraSpace(getSize());
  const uint32_t NumParameters = Parameters.size();
  const uint32_t Zero = 0;

  support::endian::write(BOS, Header.Version, llvm::endianness::little);
  support::endian::write(BOS, NumParameters, llvm::endianness::little);

  // Root Parameters offset should always start after the
  support::endian::write(BOS, (uint32_t)sizeof(dxbc::RootSignatureHeader),
                         llvm::endianness::little);

  support::endian::write(BOS, Zero, llvm::endianness::little);
  support::endian::write(BOS, Zero, llvm::endianness::little);
  support::endian::write(BOS, Header.Flags, llvm::endianness::little);

  SmallVector<uint32_t> ParamsOffsets;
  for (const auto &P : Parameters) {
    support::endian::write(BOS, P.ParameterType, llvm::endianness::little);
    support::endian::write(BOS, P.ShaderVisibility, llvm::endianness::little);

    ParamsOffsets.push_back(writePlaceholder(BOS));
  }

  assert(NumParameters == ParamsOffsets.size());
  for (size_t I = 0; I < NumParameters; ++I) {
    rewriteOffset(BOS, ParamsOffsets[I]);
    const auto &P = Parameters[I];

    switch (P.ParameterType) {
    case dxbc::RootParameterType::Constants32Bit: {
      support::endian::write(BOS, P.Constants.ShaderRegister,
                             llvm::endianness::little);
      support::endian::write(BOS, P.Constants.RegisterSpace,
                             llvm::endianness::little);
      support::endian::write(BOS, P.Constants.Num32BitValues,
                             llvm::endianness::little);
    } break;
    case dxbc::RootParameterType::CBV:
    case dxbc::RootParameterType::SRV:
    case dxbc::RootParameterType::UAV: {
      if (Header.Version == 1) {
        support::endian::write(BOS, P.DescriptorV10.ShaderRegister,
                               llvm::endianness::little);
        support::endian::write(BOS, P.DescriptorV10.RegisterSpace,
                               llvm::endianness::little);
      } else if (Header.Version == 2) {
        support::endian::write(BOS, P.DescriptorV11.ShaderRegister,
                               llvm::endianness::little);
        support::endian::write(BOS, P.DescriptorV11.RegisterSpace,
                               llvm::endianness::little);
        support::endian::write(BOS, P.DescriptorV11.Flags,
                               llvm::endianness::little);
      }

    } break;
    case llvm::dxbc::RootParameterType::DescriptorTable: {
      support::endian::write(BOS, (uint32_t)P.DescriptorTable.size(),
                             llvm::endianness::little);
      uint32_t DescTableOffset = writePlaceholder(BOS);
      rewriteOffset(BOS, DescTableOffset);

      for (size_t I = 0; I < P.DescriptorTable.size(); I++) {
        const auto &DT = P.DescriptorTable[I];
        if (Header.Version == 1) {
          support::endian::write(BOS, DT.RangesV10.RangeType,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV10.NumDescriptors,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV10.BaseShaderRegister,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV10.RegisterSpace,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV10.RegisterSpace,
                                 llvm::endianness::little);
        } else if (Header.Version == 2) {
          support::endian::write(BOS, DT.RangesV11.RangeType,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV11.NumDescriptors,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV11.BaseShaderRegister,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV11.RegisterSpace,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV11.RegisterSpace,
                                 llvm::endianness::little);
          support::endian::write(BOS, DT.RangesV11.Flags,
                                 llvm::endianness::little);
        }
        // Offset In Descriptors From Table Start
        support::endian::write(BOS, (uint32_t)I, llvm::endianness::little);
      }
    } break;
    case dxbc::RootParameterType::Empty:
      llvm_unreachable("Invalid RootParameterType");
    }
  }

  OS.write(Storage.data(), Storage.size());
}
