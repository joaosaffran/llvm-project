//===-- Breakpoint.h --------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TOOLS_LLDB_DAP_BREAKPOINT_H
#define LLDB_TOOLS_LLDB_DAP_BREAKPOINT_H

#include "BreakpointBase.h"
#include "DAPForward.h"
#include "lldb/API/SBBreakpoint.h"

namespace lldb_dap {

struct Breakpoint : public BreakpointBase {
  // The LLDB breakpoint associated wit this source breakpoint
  lldb::SBBreakpoint bp;

  Breakpoint(DAP &d, const llvm::json::Object &obj) : BreakpointBase(d, obj) {}
  Breakpoint(DAP &d, lldb::SBBreakpoint bp) : BreakpointBase(d), bp(bp) {}

  void SetCondition() override;
  void SetHitCondition() override;
  void CreateJsonObject(llvm::json::Object &object) override;

  bool MatchesName(const char *name);
  void SetBreakpoint();
};
} // namespace lldb_dap

#endif
