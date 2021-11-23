//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#include "hls/parser/dfc/stream.h"
#include "hls/parser/dfc/internal/builder.h"

using namespace eda::hls::parser::dfc;

namespace dfc {

void wire::declare(const wire *var) const {
  Builder::get().declareWire(var);
}

void wire::connect(const wire *in, const wire *out) const {
  Builder::get().connectWires(in, out);
}

void wire::connect(const std::string &opcode,
                   const std::vector<const wire*> &in,
                   const std::vector<const wire*> &out) const {
  Builder::get().connectWires(opcode, in, out);
}

} // namespace dfc
