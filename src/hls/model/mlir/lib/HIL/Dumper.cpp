//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//
//
#include "mlir/InitAllDialects.h"

#include <cassert>
#include <fstream>

#include "HIL/Dialect.h"
#include "HIL/Dumper.h"
#include "HIL/Ops.h"
#include "hls/model/model.h"
#include "hls/parser/hil/builder.h"

using eda::hls::model::Chan;
using eda::hls::model::Graph;
using eda::hls::model::Model;
using eda::hls::model::Node;
using eda::hls::model::NodeType;
using ModelPort = eda::hls::model::Port;

namespace detail {

template <typename T>
class quoted {
public:
  quoted(const T &data) : data_(data) {}
  template <typename U>
  friend std::ostream &operator<<(std::ostream &, const quoted<U> &q);

private:
  const T &data_;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const quoted<T> &q) {
  return os << '"' << q.data_ << '"';
}

template <typename T>
class curly_braced {
public:
  curly_braced(const T &data) : data_(data) {}
  template <typename U>
  friend std::ostream &operator<<(std::ostream &, const curly_braced<U> &q);

private:
  const T &data_;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const curly_braced<T> &q) {
  return os << '<' << q.data_ << '>';
}

class indented_ostream {
public:
  indented_ostream(std::ostream &os, size_t indent_step, char indent_char)
      : os_(os), indent_char_(indent_char), indent_step_(indent_step) {}

  void increase_indent() { indent_width_ += indent_step_; }

  void decrease_indent() {
    assert(indent_width_ >= indent_step_);
    indent_width_ -= indent_step_;
  }

  template <typename T>
  indented_ostream &operator<<(const T &x) {
    if (is_line_beginning_) {
      os_ << std::string(indent_width_, indent_char_);
      is_line_beginning_ = false;
    }
    os_ << x;
    return *this;
  }

  indented_ostream &operator<<(const char &x) {
    if (is_line_beginning_) {
      os_ << std::string(indent_width_, indent_char_);
      is_line_beginning_ = false;
    }
    os_ << x;
    if (x == '\n') {
      is_line_beginning_ = true;
    }
    return *this;
  }

  indented_ostream &operator<<(const char *const &s) {
    if (is_line_beginning_) {
      os_ << std::string(indent_width_, indent_char_);
      is_line_beginning_ = false;
    }
    std::string str(s);
    os_ << s;
    if (!str.empty() && str.back() == '\n') {
      is_line_beginning_ = true;
    }
    return *this;
  }

private:
  std::ostream &os_;
  std::size_t indent_width_ = 0;
  bool is_line_beginning_ = true;
  const char indent_char_;
  const std::size_t indent_step_;
};

class indent_block {
public:
  indent_block(indented_ostream &os) : os_(os) { os_.increase_indent(); }
  ~indent_block() { os_.decrease_indent(); }

private:
  indented_ostream &os_;
};

template <typename T>
class ModelDumper {
public:
  ModelDumper(const T &node, indented_ostream &os) : node_(node), os_(os) {}
  void dump();
  template <typename U>
  static ModelDumper<U> get(const U &node, indented_ostream &os) {
    return ModelDumper<U>(node, os);
  }

private:
  const T &node_;
  indented_ostream &os_;
};

enum class PortType { Input, Output };
template <PortType>
struct Port {
  Port(const ModelPort &model_port) : model_port(model_port) {}
  const ModelPort &model_port;
};
using InputPort = Port<PortType::Input>;
using OutputPort = Port<PortType::Output>;

template <>
void ModelDumper<Port<PortType::Input>>::dump() {
  const auto &port = node_.model_port;
  os_ << "!hil.input<" << quoted(port.type) << curly_braced(port.flow) << ' '
      << quoted(port.name) << ">";
}

template <>
void ModelDumper<Port<PortType::Output>>::dump() {
  const auto &port = node_.model_port;
  os_ << "!hil.output<" << quoted(port.type) << curly_braced(port.flow) << ' '
      << port.latency << ' ' << quoted(port.name);
  if (port.isConst) {
    os_ << " = " << port.value;
  }
  os_ << ">";
}

template <>
void ModelDumper<NodeType>::dump() {
  os_ << "hil.nodetype " << quoted(node_.name) << " [";
  {
    indent_block _(os_);
    bool print_sep = false;
    for (auto input_arg : node_.inputs) {
      if (print_sep) {
        os_ << ", ";
      } else {
        os_ << '\n';
        print_sep = true;
      }
      ModelDumper::get(InputPort(*input_arg), os_).dump();
    }
  }
  os_ << '\n' << "] => [";
  {
    indent_block _(os_);
    bool print_sep = false;
    for (auto output_arg : node_.outputs) {
      if (print_sep) {
        os_ << ", ";
      } else {
        os_ << '\n';
        print_sep = true;
      }
      print_sep = true;
      ModelDumper::get(OutputPort(*output_arg), os_).dump();
    }
  }
  os_ << '\n' << "]";
}

template <>
void ModelDumper<Chan>::dump() {
  os_ << "hil.chan " << quoted(node_.type) << ' ' << quoted(node_.name);
}

template <>
void ModelDumper<Node>::dump() {
  os_ << "hil.node " << quoted(node_.type.name) << ' ' << quoted(node_.name)
      << " [";
  {
    indent_block _(os_);
    bool print_sep = false;
    for (auto input_chan : node_.inputs) {
      if (print_sep) {
        os_ << ", ";
      } else {
        os_ << '\n';
        print_sep = true;
      }
      os_ << quoted(input_chan->name);
    }
  }
  os_ << '\n' << "] => [";
  {
    indent_block _(os_);
    bool print_sep = false;
    for (auto output_chan : node_.outputs) {
      if (print_sep) {
        os_ << ", ";
      } else {
        os_ << '\n';
        print_sep = true;
      }
      os_ << quoted(output_chan->name);
    }
  }
  os_ << '\n' << "]";
}

template <>
void ModelDumper<Graph>::dump() {
  os_ << "hil.graph " << quoted(node_.name) << "{\n";
  {
    indent_block _(os_);
    os_ << "hil.chans {\n";
    {
      indent_block _(os_);
      for (auto chan : node_.chans) {
        ModelDumper::get(*chan, os_).dump();
        os_ << '\n';
      }
    }
    os_ << "}\n";
    os_ << "hil.nodes {\n";
    {
      indent_block _(os_);
      for (auto node : node_.nodes) {
        ModelDumper::get(*node, os_).dump();
        os_ << '\n';
      }
    }
    os_ << "}\n";
  }
  os_ << "}\n";
}

template <>
void ModelDumper<Model>::dump() {
  os_ << "hil.model " << quoted(node_.name) << " {" << '\n';
  {
    indent_block _(os_);
    os_ << "hil.nodetypes {\n";
    {
      indent_block _(os_);
      for (auto nodetype : node_.nodetypes) {
        ModelDumper::get(*nodetype, os_).dump();
        os_ << '\n';
      }
    }
    os_ << "}\n";
    for (auto graph : node_.graphs) {
      ModelDumper::get(*graph, os_).dump();
    }
  }
  os_ << "}\n";
}

} // namespace detail

namespace eda::hls::model {
std::ostream &dump_model_mlir(const eda::hls::model::Model &model,
                              std::ostream &os) {
  detail::indented_ostream ios(os, 2, ' ');
  detail::ModelDumper(model, ios).dump();
  return os;
}

void dump_model_mlir_to_file(const eda::hls::model::Model &model,
                             const std::string &filename) {
  std::ofstream os(filename);
  dump_model_mlir(model, os);
}
} // namespace eda::hls::model
