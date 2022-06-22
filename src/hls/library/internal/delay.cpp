//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#include "hls/library/element_internal.h"
#include "hls/library/internal/delay.h"


#include <cmath>

namespace eda::hls::library {

void Delay::estimate(
    const Parameters &params, Indicators &indicators) const {
  unsigned inputCount = 0;
  unsigned latencySum = 0;
  unsigned widthSum = 0;

  unsigned latency = params.getValue(depth);

  unsigned width_value = params.getValue(width);

  for (const auto &port : ports) {
    widthSum += width_value;
    if (port.direction == Port::IN)
      inputCount++;
    else
      latencySum += latency;
  }

  double S = params.getValue(depth);
  double Areg = 16.0;
  double A = S * widthSum * Areg;
  double Fmax = 500000.0;
  double P = A;
  double D = 1000000000.0 / Fmax;

  indicators.ticks = static_cast<unsigned>(S);
  indicators.power = static_cast<unsigned>(P);
  indicators.area  = static_cast<unsigned>(A);
  indicators.delay = static_cast<unsigned>(D);

  ChanInd chanInd;
  chanInd.ticks = indicators.ticks;
  chanInd.delay = indicators.delay;

  indicators.outputs.clear();
  for (const auto &port : ports) {
    if (port.direction != Port::IN) {
      indicators.outputs.insert({ port.name, chanInd });
    }
  }
}

std::shared_ptr<MetaElement> Delay::create(const NodeType &nodetype) {
  std::string name = nodetype.name;
  //If there is no such component in the library then it has to be an internal component.
    std::shared_ptr<MetaElement> metaElement;
    auto ports = createPorts(nodetype);
    std::string lowerCaseName = name;
    unsigned i = 0;
    while (lowerCaseName[i]) {
      lowerCaseName[i] = tolower(lowerCaseName[i]);
      i++;
    }
    Parameters params;
    params.add(Parameter(depth, Constraint<unsigned>(1, std::numeric_limits<unsigned>::max()), 1));
    params.add(Parameter(width, Constraint<unsigned>(1, std::numeric_limits<unsigned>::max()), 1));

    metaElement = std::shared_ptr<MetaElement>(new Delay(lowerCaseName,
                                                         params,
                                                         ports));
  return metaElement;
};

std::unique_ptr<Element> Delay::construct(
    const Parameters &params) const {
  std::unique_ptr<Element> element = std::make_unique<Element>(ports);
  std::string inputs, outputs, ifaceWires, regs, fsm, assigns;
  std::string outputType;

  outputType = std::string("reg ");

  for (auto port : ports) {
    if (port.name == "clock" || port.name == "reset") {
      ifaceWires += std::string("input ") + port.name + ";\n";
      continue;
    }

    std::string portDeclr =
      (port.width > 1 ? std::string("[") + std::to_string(port.width - 1) + ":0] " :
                        std::string("")) + port.name + ";\n";

    if (port.direction == Port::IN || port.direction == Port::INOUT) {
      if (port.direction == Port::IN) {
        ifaceWires += std::string("input ") + portDeclr;
      } else {
        ifaceWires += std::string("inout ") + portDeclr;
      }
      inputs += std::string("wire ") + portDeclr;
    }

    if (port.direction == Port::OUT || port.direction == Port::INOUT) {
      if (port.direction == Port::OUT) {
        ifaceWires += std::string("output ") + portDeclr;
      }
      outputs += outputType + portDeclr;
    }
  }

  std::string ir;
  std::string inPort, outPort;
  unsigned d = 3; // FIXME
  regs += std::string("reg [31:0] state;\n");

  for (auto port : ports) {
    if (port.name == "clock" || port.name == "reset") {
      continue;
    }
    if (port.direction == Port::IN || port.direction == Port::INOUT) {
      inPort = port.name;
    }
    if (port.direction == Port::OUT || port.direction == Port::INOUT) {
      outPort = port.name;
    }
  }

  ir += std::string(" if (state == 0) begin\n  state <= 1;\n  s0 <= ") + inPort + "; end\nelse";
  regs += std::string("reg [31:0] s0;\n");
  for (unsigned i = 1; i < d; i++) {
    regs += std::string("reg [31:0] s") + std::to_string(i) + ";\n";
    ir += std::string(" if (state == ") + std::to_string(i) + ") begin\n";
    ir += std::string("  state <= ") + std::to_string(i + 1) + ";\n";
    ir += std::string("  s") + std::to_string(i) + " <= s" + std::to_string(i - 1) + "; end\nelse";
  }
  ir += std::string(" begin\n  state <= 0;\n  ") + outPort + " <= s" +
        std::to_string(d - 1) + "; end\nend\n";
  regs += std::string("always @(posedge clock) begin\nif (!reset) begin\n  state <= 0; end\nelse");
  element->ir = std::string("\n") + ifaceWires + inputs + outputs + regs + ir;
  //return element;
  return element;
}

} // namespace eda::hls::library
