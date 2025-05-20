#include <pch.hpp>
#include <result.hpp>
#include <util.hpp>

std::string _argsShift(int* argc, char*** argv) {
  if (*argc <= 0) {
    return "";
  }

  --(*argc);
  return *(*argv)++;
}

Result<std::string, std::string> readFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    return Result<std::string, std::string>::error(fmt::format("could not open {}", filename));
    exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return Result<std::string, std::string>::success(buffer.str());
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}