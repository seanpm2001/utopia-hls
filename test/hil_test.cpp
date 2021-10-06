//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#include <iostream>

#include "gtest/gtest.h"

#include "hls/model/model.h"
#include "hls/parser/hil/builder.h"
#include "hls/parser/hil/parser.h"

using namespace eda::hls::model;
using namespace eda::hls::parser::hil;

std::unique_ptr<Model> get_model(const std::string &filename) {
  if (parse(filename) == -1) {
    std::cout << "Could not parse " << filename << std::endl;
    return NULL;
  }

  return Builder::get().create();
}

int hil_test(const std::string &filename) {

  std::cout << *get_model(filename);

  return 0;
}

int hil_test_nodetypes(const std::string &filename) {

  return (get_model(filename))->nodetypes.size();
}

int hil_test_graphs(const std::string &filename) {

  return (get_model(filename))->graphs.size();
}

TEST(HilTest, SingleTest) {
  EXPECT_EQ(hil_test("test/hil/test.hil"), 0);
}

TEST(HilNodeTypesTest, SingleTest) {
  EXPECT_EQ(hil_test_nodetypes("test/hil/test.hil"), 6);
}

TEST(HilGraphsTest, SingleTest) {
  EXPECT_EQ(hil_test_graphs("test/hil/test.hil"), 1);
}
