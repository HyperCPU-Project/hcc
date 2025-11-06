#pragma once

#include <pch.hpp>

namespace hcc {
  class Value;

  typedef struct ABIMetadata {
    std::string return_register;
    std::vector<std::string> args_registers;
    std::vector<std::string> registers;
  } ABIMetadata;

  typedef struct TypeMetadata {
    std::string name;
    uint8_t size; // in bytes

    TypeMetadata();
    TypeMetadata(std::string name, uint8_t size);
  } TypeMetadata;

  typedef struct VariableMetadata {
    size_t offset;
    TypeMetadata type;

    VariableMetadata();
    VariableMetadata(size_t offset, TypeMetadata type);
  } VariableMetadata;

  typedef struct FunctionMetadata {
    std::string name;
    uint64_t align;
    std::map<std::string, VariableMetadata> variables;
  } FunctionMetadata;
} // namespace hcc
