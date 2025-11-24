#pragma once
#include <cstd_pch.hpp>
#include <dep_pch.hpp>

namespace hcc {
  struct ParserVarType {
    std::string name;
    bool register_ = false;

    friend std::ostream& operator<<(std::ostream& os, ParserVarType const& c) {
      return os << fmt::format("(name: {} register: {})", c.name, c.register_) << std::endl;
    }
  };
} // namespace hcc