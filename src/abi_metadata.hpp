#pragma once

#include <pch.hpp>

namespace hcc {
  typedef struct ABIMetadata {
    std::string return_register;
    std::vector<std::string> args_registers;
  } ABIMetadata;
} // namespace hcc
