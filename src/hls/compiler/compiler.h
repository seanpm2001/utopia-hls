//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>

#include "hls/model/model.h"

namespace eda::hls::compiler {

struct Compiler final {
  Compiler(const eda::hls::model::Model &model) :
    model(model) {}

  void printChan(std::ostream &out, const eda::hls::model::Chan *chan) const;
  void print(std::ostream &out) const;

  const eda::hls::model::Model model;
};

std::ostream& operator <<(std::ostream &out, const Compiler &compiler);

// TODO: Generate base modules for NodeTypes.
// TODO: Generate complex modules for Graphs.

} // namespace eda::hls::compiler
