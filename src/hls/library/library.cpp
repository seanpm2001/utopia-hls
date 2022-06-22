//===----------------------------------------------------------------------===//
//
// Part of the Utopia EDA Project, under the Apache License v2.0
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 ISP RAS (http://www.ispras.ru)
//
//===----------------------------------------------------------------------===//

#include "hls/library/element_internal.h"
#include "hls/library/ipxact_parser.h"
#include "hls/library/library.h"
#include "hls/mapper/config/hwconfig.h"
#include "util/assert.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

using namespace eda::hls::mapper::config::hwconfig;

namespace eda::hls::library {

bool MetaElement::supports(const HWConfig &hwconfig) {
  return true;
}

void Library::initialize(const std::string &libraryPath,
                         const std::string &catalogPath) {
  IPXACTParser::get().initialize();
  IPXACTParser::get().parseCatalog(libraryPath, catalogPath);
}

void Library::finalize() {
  IPXACTParser::get().finalize();
}

void Library::importLibrary(const std::string &libraryPath,
                            const std::string &catalogPath) {
  IPXACTParser::get().parseCatalog(libraryPath, catalogPath);
}

std::shared_ptr<MetaElement> Library::find(const NodeType &nodetype,
                                           const HWConfig &hwconfig) {
  const auto name = nodetype.name;

  const auto i = std::find_if(cache.begin(), cache.end(),
    [&name, &hwconfig](const std::shared_ptr<MetaElement> &metaElement) {
      return metaElement->name == name && metaElement->supports(hwconfig);
    });

  if (i != cache.end()) {
    return *i;
  }

  if (IPXACTParser::get().hasComponent(name, hwconfig)) {
    auto metaElement = IPXACTParser::get().parseComponent(name);
    cache.push_back(metaElement);
    return metaElement;
  }
  auto metaElement = ElementInternal::create(nodetype, hwconfig);
  cache.push_back(metaElement);
  return metaElement;
}

} // namespace eda::hls::library
