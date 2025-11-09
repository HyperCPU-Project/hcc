#pragma once

#include <pch.hpp>
#include <value/value.hpp>

namespace hcc {
  typedef struct FunctionMetadata {
    std::string name;
    uint64_t align;
    std::map<std::string, std::unique_ptr<Value>> variables;
  } FunctionMetadata;
} // namespace hcc
