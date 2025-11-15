#include <cstd_pch.hpp>
#include <util.hpp>

std::string _ArgsShift(int* argc, char*** argv) {
  if (*argc <= 0) {
    return "";
  }

  --(*argc);
  return *(*argv)++;
}

tl::expected<std::string, std::string> ReadFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    return tl::unexpected(fmt::format("could not open {}", filename));
    exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return {buffer.str()};
}

bool Replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
