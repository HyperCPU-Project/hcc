#pragma once

#include <cstd_pch.hpp>

namespace hcc {
  typedef struct ABIMetadata {
    std::string return_register;
    std::vector<std::string> args_registers;
    std::vector<std::string> registers;
  } ABIMetadata;
} // namespace hcc
