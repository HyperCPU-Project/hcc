#pragma once

#include <string>
#include <type_metadata.hpp>

namespace hcc {
  struct ValueStackVar {
    uint64_t stack_align;
    std::string name;
    TypeMetadata type;
  };
} // namespace hcc
