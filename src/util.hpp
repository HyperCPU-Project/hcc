#pragma once
#include <result.hpp>

#define argsShift(...) _argsShift(&argc, &argv)

char* _argsShift(int* argc, char*** argv);
Result<std::string, std::string> readFile(const std::string& filename);
bool replace(std::string& str, const std::string& from, const std::string& to);
