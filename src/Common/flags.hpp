#pragma once
#include <pch.hpp>

namespace hcc {
  template <typename T = unsigned char>
  class Flags {
  private:
    std::vector<T> flags;

  public:
    Flags() = default;

    void SetFlag(T v) {
      flags.push_back(v);
    }

    bool UnsetFlag(T v) {
      for (size_t i = 0; i < flags.size(); i++) {
        if (flags[i] == v) {
          flags.erase(flags.begin() + i);
          return true;
        }
      }
      return false;
    }

    void FlipFlag(T v) {
      if (!UnsetFlag(v))
        SetFlag(v);
    }

    bool HasFlag(T v) {
      for (size_t i = 0; i < flags.size(); i++) {
        if (flags[i] == v) {
          return true;
        }
      }
      return false;
    }
  };
} // namespace hcc
