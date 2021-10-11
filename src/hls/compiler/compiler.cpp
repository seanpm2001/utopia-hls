//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#include <memory>

#include <hls/compiler/compiler.h>
#include <hls/library/library.h>

namespace eda::hls::compiler {

// TODO: move this code into VerilogPrinter
void Compiler::printChan(std::ostream &out, const eda::hls::model::Chan *chan) const {
  // TODO: chan.type is ignored
  // out << chan->name << " " << " source: " << (chan->source != nullptr ? chan->source->name : "") <<
  //                             " target: " << (chan->target != nullptr ? chan->target->name : "") << std::endl;
  out << "." << chan->target->name << "(" << chan->source->name << ")";
}

void Compiler::print(std::ostream &out) const {
  // print model.nodetypes
  for (const auto *nodetype : model.nodetypes) {
    auto printer = std::make_unique<library::VerilogPrinter>(*nodetype);
    out << *printer;
  }

  // print model.graphs TODO: refactor it by moving to VerilogPrinter!
  for (const auto *graph : model.graphs) {
    out << "module " << graph->name << " _";
    for (auto elem : graph->name) {
      out << (char)std::tolower((unsigned char)elem);
    }
    out << " ();" << std::endl;

    for (const auto *chan : graph->chans) {
      out << "wire " << chan->name << ";" << std::endl;
      // printChan(out, chan);
    }
    for (const auto *node : graph->nodes) {
      // TODO: node.type is ignored
      out << node->type.name << " _" << node->type.name << "(";

      bool comma = false;

      for (const auto *input : node->inputs) {
        out << (comma ? "," : "");
        printChan(out, input);
      }

      for (const auto *output: node->outputs) {
        out << (comma ? "," : "");
        printChan(out, output);
      }
      out << ");" << std::endl;
    }
    out << "endmodule // " << graph->name << std::endl;
  }
}

std::ostream& operator <<(std::ostream &out, const Compiler &compiler) {
  compiler.print(out);
  return out;
}

} // namespace eda::hls::compiler
