#pragma once

#include <pch.hpp>

namespace hcc {
  class Value;

  typedef struct ABIMetadata {
    std::string return_register;
    std::vector<std::string> args_registers;
  } ABIMetadata;

  typedef struct TypeMetadata {
    std::string name;
    uint8_t size; // in bytes
  } TypeMetadata;

  typedef struct FunctionMetadata {
    std::string name;
    uint64_t align;
    std::map<std::string, std::unique_ptr<Value>> variables;
  } FunctionMetadata;
} // namespace hcc
