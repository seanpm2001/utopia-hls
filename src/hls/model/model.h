//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace eda::hls::model {

struct Argument final {
  Argument(const std::string &name, const std::string &type, float flow):
    name(name), type(type), flow(flow) {}

  std::string name;
  std::string type;
  float flow;
};

struct NodeType final {
  NodeType(const std::string &name, unsigned latency):
    name(name), latency(latency) {}

  void add_input(Argument *input) {
    inputs.push_back(input);
  }

  void add_output(Argument *output) {
    outputs.push_back(output);
  }

  std::string name;
  unsigned latency;
  std::vector<Argument *> inputs;
  std::vector<Argument *> outputs;
};

struct Chan final {
  Chan(const std::string &name, const std::string &type):
    name(name), type(type) {}

  std::string name;
  std::string type;
  const Argument *source = nullptr;
  const Argument *target = nullptr;
};

struct Node final {
  Node(const NodeType &type):
    type(type) {}

  void add_input(Chan *input) {
    inputs.push_back(input);
  }

  void add_output(Chan *output) {
    outputs.push_back(output);
  }

  const NodeType &type;
  std::vector<Chan *> inputs;
  std::vector<Chan *> outputs;
};

struct Graph final {
  Graph(const std::string &name):
    name(name) {}

  void add_chan(Chan *chan) {
    chans.push_back(chan);
  }

  void add_node(Node *node) {
    nodes.push_back(node);
  }

  std::string name;
  std::vector<Chan *> chans;
  std::vector<Node *> nodes;
};

struct Model final {
  Model() = default;

  void add_nodetype(NodeType *nodetype) {
    nodetypes.push_back(nodetype);
  }

  void add_graph(Graph *graph) {
    graphs.push_back(graph);
  }

  std::vector<NodeType *> nodetypes;
  std::vector<Graph *> graphs;
};

std::ostream& operator <<(std::ostream &out, const Argument &argument);
std::ostream& operator <<(std::ostream &out, const NodeType &nodetype);
std::ostream& operator <<(std::ostream &out, const Chan &chan);
std::ostream& operator <<(std::ostream &out, const Node &node);
std::ostream& operator <<(std::ostream &out, const Graph &graph);
std::ostream& operator <<(std::ostream &out, const Model &model);

} // namespace eda::hls::model
