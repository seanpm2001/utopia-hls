//===----------------------------------------------------------------------===//
//
// Part of the Utopia HLS Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021-2024 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#ifndef DFCIR_PASSES_H
#define DFCIR_PASSES_H

#include "dfcir/DFCIROperations.h"
#include "mlir/Pass/Pass.h"

#include "memory"

namespace mlir::dfcir {

enum Ops {
  UNDEFINED,
  ADD_INT,
  ADD_FLOAT,
  SUB_INT,
  SUB_FLOAT,
  MUL_INT,
  MUL_FLOAT,
  DIV_INT,
  DIV_FLOAT,
  AND_INT,
  AND_FLOAT,
  OR_INT,
  OR_FLOAT,
  XOR_INT,
  XOR_FLOAT,
  NOT_INT,
  NOT_FLOAT,
  NEG_INT,
  NEG_FLOAT,
  LESS_INT,
  LESS_FLOAT,
  LESSEQ_INT,
  LESSEQ_FLOAT,
  GREATER_INT,
  GREATER_FLOAT,
  GREATEREQ_INT,
  GREATEREQ_FLOAT,
  EQ_INT,
  EQ_FLOAT,
  NEQ_INT,
  NEQ_FLOAT,
  COUNT
};

} // namespace mlir::dfcir

typedef std::unordered_map<mlir::dfcir::Ops, unsigned> LatencyConfig;

namespace mlir::dfcir {

using std::unique_ptr;
using mlir::Pass;

unique_ptr<Pass> createDFCIRToFIRRTLPass(LatencyConfig *config = nullptr);

unique_ptr<Pass> createDFCIRASAPSchedulerPass();

unique_ptr<Pass> createDFCIRLinearSchedulerPass();

} // namespace mlir::dfcir

#define GEN_PASS_REGISTRATION

#include "dfcir/conversions/DFCIRPasses.h.inc"

#endif // DFCIR_PASSES_H
