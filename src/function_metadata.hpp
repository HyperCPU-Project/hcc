#pragma once

#include <cstd_pch.hpp>
#include <value/value.hpp>

namespace hcc {
  typedef struct FunctionMetadata {
    std::string name;
    uint64_t align;
    std::map<std::string, std::shared_ptr<Value>> variables;
  } FunctionMetadata;
} // namespace hcc
