/*
 * Copyright 2021 ISP RAS (http://www.ispras.ru)
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

#include <iostream>

#include "gate/gate.h"

namespace eda {
namespace gate {

static std::ostream& operator <<(std::ostream &out, const std::vector<Gate *> &gates) {
  bool separator = false;
  for (Gate *gate: gates) {
    out << (separator ? ", " : "") << gate->id();
    separator = true;
  }

  return out;
}

std::ostream& operator <<(std::ostream &out, const Gate &gate) {
  if (gate.is_source()) {
    return out << "S{" << gate.id() << "}";
  } else if (gate.is_gate()) {
    return out << "G{" << gate.id() << " <= " << gate.gate() << "(" << gate._inputs << ")}";
  } else {
    return out << "T{clk=" << gate.clock() << ", rst=" << gate.reset() << ": "
               << gate.id() << " <= " << gate.input(0)->id() << "}";
  }
}

}} // namespace eda::gate

