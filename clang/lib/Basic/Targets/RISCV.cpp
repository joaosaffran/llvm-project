//===--- RISCV.cpp - Implement RISC-V target feature support --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements RISC-V TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "RISCV.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/RISCVTargetParser.h"
#include <optional>

using namespace clang;
using namespace clang::targets;

ArrayRef<const char *> RISCVTargetInfo::getGCCRegNames() const {
  // clang-format off
  static const char *const GCCRegNames[] = {
      // Integer registers
      "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",
      "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
      "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
      "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31",

      // Floating point registers
      "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
      "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15",
      "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
      "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",

      // Vector registers
      "v0",  "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",
      "v8",  "v9",  "v10", "v11", "v12", "v13", "v14", "v15",
      "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
      "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",

      // CSRs
      "fflags", "frm", "vtype", "vl", "vxsat", "vxrm", "sf.vcix_state"
    };
  // clang-format on
  return llvm::ArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> RISCVTargetInfo::getGCCRegAliases() const {
  static const TargetInfo::GCCRegAlias GCCRegAliases[] = {
      {{"zero"}, "x0"}, {{"ra"}, "x1"},   {{"sp"}, "x2"},    {{"gp"}, "x3"},
      {{"tp"}, "x4"},   {{"t0"}, "x5"},   {{"t1"}, "x6"},    {{"t2"}, "x7"},
      {{"s0"}, "x8"},   {{"s1"}, "x9"},   {{"a0"}, "x10"},   {{"a1"}, "x11"},
      {{"a2"}, "x12"},  {{"a3"}, "x13"},  {{"a4"}, "x14"},   {{"a5"}, "x15"},
      {{"a6"}, "x16"},  {{"a7"}, "x17"},  {{"s2"}, "x18"},   {{"s3"}, "x19"},
      {{"s4"}, "x20"},  {{"s5"}, "x21"},  {{"s6"}, "x22"},   {{"s7"}, "x23"},
      {{"s8"}, "x24"},  {{"s9"}, "x25"},  {{"s10"}, "x26"},  {{"s11"}, "x27"},
      {{"t3"}, "x28"},  {{"t4"}, "x29"},  {{"t5"}, "x30"},   {{"t6"}, "x31"},
      {{"ft0"}, "f0"},  {{"ft1"}, "f1"},  {{"ft2"}, "f2"},   {{"ft3"}, "f3"},
      {{"ft4"}, "f4"},  {{"ft5"}, "f5"},  {{"ft6"}, "f6"},   {{"ft7"}, "f7"},
      {{"fs0"}, "f8"},  {{"fs1"}, "f9"},  {{"fa0"}, "f10"},  {{"fa1"}, "f11"},
      {{"fa2"}, "f12"}, {{"fa3"}, "f13"}, {{"fa4"}, "f14"},  {{"fa5"}, "f15"},
      {{"fa6"}, "f16"}, {{"fa7"}, "f17"}, {{"fs2"}, "f18"},  {{"fs3"}, "f19"},
      {{"fs4"}, "f20"}, {{"fs5"}, "f21"}, {{"fs6"}, "f22"},  {{"fs7"}, "f23"},
      {{"fs8"}, "f24"}, {{"fs9"}, "f25"}, {{"fs10"}, "f26"}, {{"fs11"}, "f27"},
      {{"ft8"}, "f28"}, {{"ft9"}, "f29"}, {{"ft10"}, "f30"}, {{"ft11"}, "f31"}};
  return llvm::ArrayRef(GCCRegAliases);
}

bool RISCVTargetInfo::validateAsmConstraint(
    const char *&Name, TargetInfo::ConstraintInfo &Info) const {
  switch (*Name) {
  default:
    return false;
  case 'I':
    // A 12-bit signed immediate.
    Info.setRequiresImmediate(-2048, 2047);
    return true;
  case 'J':
    // Integer zero.
    Info.setRequiresImmediate(0);
    return true;
  case 'K':
    // A 5-bit unsigned immediate for CSR access instructions.
    Info.setRequiresImmediate(0, 31);
    return true;
  case 'f':
    // A floating-point register.
    Info.setAllowsRegister();
    return true;
  case 'A':
    // An address that is held in a general-purpose register.
    Info.setAllowsMemory();
    return true;
  case 's':
  case 'S': // A symbol or label reference with a constant offset
    Info.setAllowsRegister();
    return true;
  case 'c':
    // A RVC register - GPR or FPR
    if (Name[1] == 'r' || Name[1] == 'R' || Name[1] == 'f') {
      Info.setAllowsRegister();
      Name += 1;
      return true;
    }
    return false;
  case 'R':
    // An even-odd GPR pair
    Info.setAllowsRegister();
    return true;
  case 'v':
    // A vector register.
    if (Name[1] == 'r' || Name[1] == 'd' || Name[1] == 'm') {
      Info.setAllowsRegister();
      Name += 1;
      return true;
    }
    return false;
  }
}

std::string RISCVTargetInfo::convertConstraint(const char *&Constraint) const {
  std::string R;
  switch (*Constraint) {
  // c* and v* are two-letter constraints on RISC-V.
  case 'c':
  case 'v':
    R = std::string("^") + std::string(Constraint, 2);
    Constraint += 1;
    break;
  default:
    R = TargetInfo::convertConstraint(Constraint);
    break;
  }
  return R;
}

static unsigned getVersionValue(unsigned MajorVersion, unsigned MinorVersion) {
  return MajorVersion * 1000000 + MinorVersion * 1000;
}

void RISCVTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__riscv");
  bool Is64Bit = getTriple().isRISCV64();
  Builder.defineMacro("__riscv_xlen", Is64Bit ? "64" : "32");
  StringRef CodeModel = getTargetOpts().CodeModel;
  unsigned FLen = ISAInfo->getFLen();
  unsigned MinVLen = ISAInfo->getMinVLen();
  unsigned MaxELen = ISAInfo->getMaxELen();
  unsigned MaxELenFp = ISAInfo->getMaxELenFp();
  if (CodeModel == "default")
    CodeModel = "small";

  if (CodeModel == "small")
    Builder.defineMacro("__riscv_cmodel_medlow");
  else if (CodeModel == "medium")
    Builder.defineMacro("__riscv_cmodel_medany");
  else if (CodeModel == "large")
    Builder.defineMacro("__riscv_cmodel_large");

  StringRef ABIName = getABI();
  if (ABIName == "ilp32f" || ABIName == "lp64f")
    Builder.defineMacro("__riscv_float_abi_single");
  else if (ABIName == "ilp32d" || ABIName == "lp64d")
    Builder.defineMacro("__riscv_float_abi_double");
  else
    Builder.defineMacro("__riscv_float_abi_soft");

  if (ABIName == "ilp32e" || ABIName == "lp64e")
    Builder.defineMacro("__riscv_abi_rve");

  Builder.defineMacro("__riscv_arch_test");

  for (auto &Extension : ISAInfo->getExtensions()) {
    auto ExtName = Extension.first;
    auto ExtInfo = Extension.second;

    Builder.defineMacro(Twine("__riscv_", ExtName),
                        Twine(getVersionValue(ExtInfo.Major, ExtInfo.Minor)));
  }

  if (ISAInfo->hasExtension("zmmul"))
    Builder.defineMacro("__riscv_mul");

  if (ISAInfo->hasExtension("m")) {
    Builder.defineMacro("__riscv_div");
    Builder.defineMacro("__riscv_muldiv");
  }

  if (ISAInfo->hasExtension("a")) {
    Builder.defineMacro("__riscv_atomic");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4");
    if (Is64Bit)
      Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8");
  }

  if (FLen) {
    Builder.defineMacro("__riscv_flen", Twine(FLen));
    Builder.defineMacro("__riscv_fdiv");
    Builder.defineMacro("__riscv_fsqrt");
  }

  if (MinVLen) {
    Builder.defineMacro("__riscv_v_min_vlen", Twine(MinVLen));
    Builder.defineMacro("__riscv_v_elen", Twine(MaxELen));
    Builder.defineMacro("__riscv_v_elen_fp", Twine(MaxELenFp));
  }

  if (ISAInfo->hasExtension("c"))
    Builder.defineMacro("__riscv_compressed");

  if (ISAInfo->hasExtension("zve32x"))
    Builder.defineMacro("__riscv_vector");

  // Currently we support the v1.0 RISC-V V intrinsics.
  Builder.defineMacro("__riscv_v_intrinsic", Twine(getVersionValue(1, 0)));

  auto VScale = getVScaleRange(Opts, ArmStreamingKind::NotStreaming);
  if (VScale && VScale->first && VScale->first == VScale->second)
    Builder.defineMacro("__riscv_v_fixed_vlen",
                        Twine(VScale->first * llvm::RISCV::RVVBitsPerBlock));

  if (FastScalarUnalignedAccess)
    Builder.defineMacro("__riscv_misaligned_fast");
  else
    Builder.defineMacro("__riscv_misaligned_avoid");

  if (ISAInfo->hasExtension("e")) {
    if (Is64Bit)
      Builder.defineMacro("__riscv_64e");
    else
      Builder.defineMacro("__riscv_32e");
  }

  if (Opts.CFProtectionReturn && ISAInfo->hasExtension("zicfiss"))
    Builder.defineMacro("__riscv_shadow_stack");

  if (Opts.CFProtectionBranch) {
    auto Scheme = Opts.getCFBranchLabelScheme();
    if (Scheme == CFBranchLabelSchemeKind::Default)
      Scheme = getDefaultCFBranchLabelScheme();

    Builder.defineMacro("__riscv_landing_pad");
    switch (Scheme) {
    case CFBranchLabelSchemeKind::Unlabeled:
      Builder.defineMacro("__riscv_landing_pad_unlabeled");
      break;
    case CFBranchLabelSchemeKind::FuncSig:
      // TODO: Define macros after the func-sig scheme is implemented
      break;
    case CFBranchLabelSchemeKind::Default:
      llvm_unreachable("default cf-branch-label scheme should already be "
                       "transformed to other scheme");
    }
  }
}

static constexpr int NumRVVBuiltins =
    RISCVVector::FirstSiFiveBuiltin - Builtin::FirstTSBuiltin;
static constexpr int NumRVVSiFiveBuiltins =
    RISCVVector::FirstAndesBuiltin - RISCVVector::FirstSiFiveBuiltin;
static constexpr int NumRVVAndesBuiltins =
    RISCVVector::FirstTSBuiltin - RISCVVector::FirstAndesBuiltin;
static constexpr int NumRISCVBuiltins =
    RISCV::LastTSBuiltin - RISCVVector::FirstTSBuiltin;
static constexpr int NumBuiltins =
    RISCV::LastTSBuiltin - Builtin::FirstTSBuiltin;
static_assert(NumBuiltins == (NumRVVBuiltins + NumRVVSiFiveBuiltins +
                              NumRVVAndesBuiltins + NumRISCVBuiltins));

namespace RVV {
#define GET_RISCVV_BUILTIN_STR_TABLE
#include "clang/Basic/riscv_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_STR_TABLE
static_assert(BuiltinStrings.size() < 100'000);

static constexpr std::array<Builtin::Info, NumRVVBuiltins> BuiltinInfos = {
#define GET_RISCVV_BUILTIN_INFOS
#include "clang/Basic/riscv_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_INFOS
};
} // namespace RVV

namespace RVVSiFive {
#define GET_RISCVV_BUILTIN_STR_TABLE
#include "clang/Basic/riscv_sifive_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_STR_TABLE

static constexpr std::array<Builtin::Info, NumRVVSiFiveBuiltins> BuiltinInfos =
    {
#define GET_RISCVV_BUILTIN_INFOS
#include "clang/Basic/riscv_sifive_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_INFOS
};
} // namespace RVVSiFive

namespace RVVAndes {
#define GET_RISCVV_BUILTIN_STR_TABLE
#include "clang/Basic/riscv_andes_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_STR_TABLE

static constexpr std::array<Builtin::Info, NumRVVAndesBuiltins> BuiltinInfos =
    {
#define GET_RISCVV_BUILTIN_INFOS
#include "clang/Basic/riscv_andes_vector_builtins.inc"
#undef GET_RISCVV_BUILTIN_INFOS
};
} // namespace RVVAndes

#define GET_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsRISCV.inc"
#undef GET_BUILTIN_STR_TABLE

static constexpr Builtin::Info BuiltinInfos[] = {
#define GET_BUILTIN_INFOS
#include "clang/Basic/BuiltinsRISCV.inc"
#undef GET_BUILTIN_INFOS
};
static_assert(std::size(BuiltinInfos) == NumRISCVBuiltins);

llvm::SmallVector<Builtin::InfosShard>
RISCVTargetInfo::getTargetBuiltins() const {
  return {
      {&RVV::BuiltinStrings, RVV::BuiltinInfos, "__builtin_rvv_"},
      {&RVVSiFive::BuiltinStrings, RVVSiFive::BuiltinInfos, "__builtin_rvv_"},
      {&RVVAndes::BuiltinStrings, RVVAndes::BuiltinInfos, "__builtin_rvv_"},
      {&BuiltinStrings, BuiltinInfos},
  };
}

bool RISCVTargetInfo::initFeatureMap(
    llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags, StringRef CPU,
    const std::vector<std::string> &FeaturesVec) const {

  unsigned XLen = 32;

  if (getTriple().isRISCV64()) {
    Features["64bit"] = true;
    XLen = 64;
  } else {
    Features["32bit"] = true;
  }

  std::vector<std::string> AllFeatures = FeaturesVec;
  auto ParseResult = llvm::RISCVISAInfo::parseFeatures(XLen, FeaturesVec);
  if (!ParseResult) {
    std::string Buffer;
    llvm::raw_string_ostream OutputErrMsg(Buffer);
    handleAllErrors(ParseResult.takeError(), [&](llvm::StringError &ErrMsg) {
      OutputErrMsg << ErrMsg.getMessage();
    });
    Diags.Report(diag::err_invalid_feature_combination) << OutputErrMsg.str();
    return false;
  }

  // Append all features, not just new ones, so we override any negatives.
  llvm::append_range(AllFeatures, (*ParseResult)->toFeatures());
  return TargetInfo::initFeatureMap(Features, Diags, CPU, AllFeatures);
}

std::optional<std::pair<unsigned, unsigned>>
RISCVTargetInfo::getVScaleRange(const LangOptions &LangOpts,
                                ArmStreamingKind IsArmStreamingFunction,
                                llvm::StringMap<bool> *FeatureMap) const {
  // RISCV::RVVBitsPerBlock is 64.
  unsigned VScaleMin = ISAInfo->getMinVLen() / llvm::RISCV::RVVBitsPerBlock;

  if (LangOpts.VScaleMin || LangOpts.VScaleMax) {
    // Treat Zvl*b as a lower bound on vscale.
    VScaleMin = std::max(VScaleMin, LangOpts.VScaleMin);
    unsigned VScaleMax = LangOpts.VScaleMax;
    if (VScaleMax != 0 && VScaleMax < VScaleMin)
      VScaleMax = VScaleMin;
    return std::pair<unsigned, unsigned>(VScaleMin ? VScaleMin : 1, VScaleMax);
  }

  if (VScaleMin > 0) {
    unsigned VScaleMax = ISAInfo->getMaxVLen() / llvm::RISCV::RVVBitsPerBlock;
    return std::make_pair(VScaleMin, VScaleMax);
  }

  return std::nullopt;
}

/// Return true if has this feature, need to sync with handleTargetFeatures.
bool RISCVTargetInfo::hasFeature(StringRef Feature) const {
  bool Is64Bit = getTriple().isRISCV64();
  auto Result = llvm::StringSwitch<std::optional<bool>>(Feature)
                    .Case("riscv", true)
                    .Case("riscv32", !Is64Bit)
                    .Case("riscv64", Is64Bit)
                    .Case("32bit", !Is64Bit)
                    .Case("64bit", Is64Bit)
                    .Case("experimental", HasExperimental)
                    .Default(std::nullopt);
  if (Result)
    return *Result;

  return ISAInfo->hasExtension(Feature);
}

/// Perform initialization based on the user configured set of features.
bool RISCVTargetInfo::handleTargetFeatures(std::vector<std::string> &Features,
                                           DiagnosticsEngine &Diags) {
  unsigned XLen = getTriple().isArch64Bit() ? 64 : 32;
  auto ParseResult = llvm::RISCVISAInfo::parseFeatures(XLen, Features);
  if (!ParseResult) {
    std::string Buffer;
    llvm::raw_string_ostream OutputErrMsg(Buffer);
    handleAllErrors(ParseResult.takeError(), [&](llvm::StringError &ErrMsg) {
      OutputErrMsg << ErrMsg.getMessage();
    });
    Diags.Report(diag::err_invalid_feature_combination) << OutputErrMsg.str();
    return false;
  } else {
    ISAInfo = std::move(*ParseResult);
  }

  if (ABI.empty())
    ABI = ISAInfo->computeDefaultABI().str();

  if (ISAInfo->hasExtension("zfh") || ISAInfo->hasExtension("zhinx"))
    HasLegalHalfType = true;

  FastScalarUnalignedAccess =
      llvm::is_contained(Features, "+unaligned-scalar-mem");

  if (llvm::is_contained(Features, "+experimental"))
    HasExperimental = true;

  if (ABI == "ilp32e" && ISAInfo->hasExtension("d")) {
    Diags.Report(diag::err_invalid_feature_combination)
        << "ILP32E cannot be used with the D ISA extension";
    return false;
  }
  return true;
}

bool RISCVTargetInfo::isValidCPUName(StringRef Name) const {
  bool Is64Bit = getTriple().isArch64Bit();
  return llvm::RISCV::parseCPU(Name, Is64Bit);
}

void RISCVTargetInfo::fillValidCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  bool Is64Bit = getTriple().isArch64Bit();
  llvm::RISCV::fillValidCPUArchList(Values, Is64Bit);
}

bool RISCVTargetInfo::isValidTuneCPUName(StringRef Name) const {
  bool Is64Bit = getTriple().isArch64Bit();
  return llvm::RISCV::parseTuneCPU(Name, Is64Bit);
}

void RISCVTargetInfo::fillValidTuneCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  bool Is64Bit = getTriple().isArch64Bit();
  llvm::RISCV::fillValidTuneCPUArchList(Values, Is64Bit);
}

static void populateNegativeRISCVFeatures(std::vector<std::string> &Features) {
  auto RII = llvm::RISCVISAInfo::parseArchString(
      "rv64i", /* EnableExperimentalExtension */ true);

  if (llvm::errorToBool(RII.takeError()))
    llvm_unreachable("unsupport rv64i");

  std::vector<std::string> FeatStrings =
      (*RII)->toFeatures(/* AddAllExtensions */ true);
  llvm::append_range(Features, FeatStrings);
}

static void handleFullArchString(StringRef FullArchStr,
                                 std::vector<std::string> &Features) {
  auto RII = llvm::RISCVISAInfo::parseArchString(
      FullArchStr, /* EnableExperimentalExtension */ true);
  if (llvm::errorToBool(RII.takeError())) {
    // Forward the invalid FullArchStr.
    Features.push_back(FullArchStr.str());
  } else {
    // Append a full list of features, including any negative extensions so that
    // we override the CPU's features.
    populateNegativeRISCVFeatures(Features);
    std::vector<std::string> FeatStrings =
        (*RII)->toFeatures(/* AddAllExtensions */ true);
    llvm::append_range(Features, FeatStrings);
  }
}

ParsedTargetAttr RISCVTargetInfo::parseTargetAttr(StringRef Features) const {
  ParsedTargetAttr Ret;
  if (Features == "default")
    return Ret;
  SmallVector<StringRef, 1> AttrFeatures;
  Features.split(AttrFeatures, ";");
  bool FoundArch = false;

  auto handleArchExtension = [](StringRef AttrString,
                                std::vector<std::string> &Features) {
    SmallVector<StringRef, 1> Exts;
    AttrString.split(Exts, ",");
    for (auto Ext : Exts) {
      if (Ext.empty())
        continue;

      StringRef ExtName = Ext.substr(1);
      std::string TargetFeature =
          llvm::RISCVISAInfo::getTargetFeatureForExtension(ExtName);
      if (!TargetFeature.empty())
        Features.push_back(Ext.front() + TargetFeature);
      else
        Features.push_back(Ext.str());
    }
  };

  for (auto &Feature : AttrFeatures) {
    Feature = Feature.trim();
    StringRef AttrString = Feature.split("=").second.trim();

    if (Feature.starts_with("arch=")) {
      // Override last features
      Ret.Features.clear();
      if (FoundArch)
        Ret.Duplicate = "arch=";
      FoundArch = true;

      if (AttrString.starts_with("+")) {
        // EXTENSION like arch=+v,+zbb
        handleArchExtension(AttrString, Ret.Features);
      } else {
        // full-arch-string like arch=rv64gcv
        handleFullArchString(AttrString, Ret.Features);
      }
    } else if (Feature.starts_with("cpu=")) {
      if (!Ret.CPU.empty())
        Ret.Duplicate = "cpu=";

      Ret.CPU = AttrString;

      if (!FoundArch) {
        // Update Features with CPU's features
        StringRef MarchFromCPU = llvm::RISCV::getMArchFromMcpu(Ret.CPU);
        if (MarchFromCPU != "") {
          Ret.Features.clear();
          handleFullArchString(MarchFromCPU, Ret.Features);
        }
      }
    } else if (Feature.starts_with("tune=")) {
      if (!Ret.Tune.empty())
        Ret.Duplicate = "tune=";

      Ret.Tune = AttrString;
    } else if (Feature.starts_with("priority")) {
      // Skip because it only use for FMV.
    } else if (Feature.starts_with("+")) {
      // Handle target_version/target_clones attribute strings
      // that are already delimited by ','
      handleArchExtension(Feature, Ret.Features);
    }
  }
  return Ret;
}

llvm::APInt
RISCVTargetInfo::getFMVPriority(ArrayRef<StringRef> Features) const {
  // Priority is explicitly specified on RISC-V unlike on other targets, where
  // it is derived by all the features of a specific version. Therefore if a
  // feature contains the priority string, then return it immediately.
  for (StringRef Feature : Features) {
    auto [LHS, RHS] = Feature.rsplit(';');
    if (LHS.consume_front("priority="))
      Feature = LHS;
    else if (RHS.consume_front("priority="))
      Feature = RHS;
    else
      continue;
    unsigned Priority;
    if (!Feature.getAsInteger(0, Priority))
      return llvm::APInt(32, Priority);
  }
  // Default Priority is zero.
  return llvm::APInt::getZero(32);
}

TargetInfo::CallingConvCheckResult
RISCVTargetInfo::checkCallingConvention(CallingConv CC) const {
  switch (CC) {
  default:
    return CCCR_Warning;
  case CC_C:
  case CC_RISCVVectorCall:
  case CC_RISCVVLSCall_32:
  case CC_RISCVVLSCall_64:
  case CC_RISCVVLSCall_128:
  case CC_RISCVVLSCall_256:
  case CC_RISCVVLSCall_512:
  case CC_RISCVVLSCall_1024:
  case CC_RISCVVLSCall_2048:
  case CC_RISCVVLSCall_4096:
  case CC_RISCVVLSCall_8192:
  case CC_RISCVVLSCall_16384:
  case CC_RISCVVLSCall_32768:
  case CC_RISCVVLSCall_65536:
    return CCCR_OK;
  }
}

bool RISCVTargetInfo::validateCpuSupports(StringRef Feature) const {
  // Only allow extensions we have a known bit position for in the
  // __riscv_feature_bits structure.
  return -1 != llvm::RISCVISAInfo::getRISCVFeaturesBitsInfo(Feature).second;
}

bool RISCVTargetInfo::isValidFeatureName(StringRef Name) const {
  return llvm::RISCVISAInfo::isSupportedExtensionFeature(Name);
}

bool RISCVTargetInfo::validateGlobalRegisterVariable(
    StringRef RegName, unsigned RegSize, bool &HasSizeMismatch) const {
  if (RegName == "ra" || RegName == "sp" || RegName == "gp" ||
      RegName == "tp" || RegName.starts_with("x") || RegName.starts_with("a") ||
      RegName.starts_with("s") || RegName.starts_with("t")) {
    unsigned XLen = getTriple().isArch64Bit() ? 64 : 32;
    HasSizeMismatch = RegSize != XLen;
    return true;
  }
  return false;
}

bool RISCVTargetInfo::validateCpuIs(StringRef CPUName) const {
  assert(getTriple().isOSLinux() &&
         "__builtin_cpu_is() is only supported for Linux.");

  return llvm::RISCV::hasValidCPUModel(CPUName);
}
