#pragma once
#include <result.hpp>

#define ArgsShift(...) _ArgsShift(&argc, &argv)

std::string _ArgsShift(int* argc, char*** argv);
Result<std::string, std::string> ReadFile(const std::string& filename);
bool Replace(std::string& str, const std::string& from, const std::string& to);