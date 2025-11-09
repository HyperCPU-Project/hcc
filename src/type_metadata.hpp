#pragma once

#include <pch.hpp>

namespace hcc {
  typedef struct TypeMetadata {
    std::string name;
    uint8_t size; // in bytes
  } TypeMetadata;
} // namespace hcc
