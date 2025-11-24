#pragma once
#include "dep_pch.hpp"

#define ArgsShift(...) _ArgsShift(&argc, &argv)

std::string _ArgsShift(int* argc, char*** argv);
tl::expected<std::string, std::string> ReadFile(const std::string& filename);
bool Replace(std::string& str, const std::string& from, const std::string& to);

template <typename T>
auto VectorSearch(const std::vector<T>& vec, const T& needle) {
  return std::find(vec.begin(), vec.end(), needle);
}
template <typename T>
bool VectorContains(const std::vector<T>& vec, const T& needle) {
  return VectorSearch(vec, needle) != vec.end();
}