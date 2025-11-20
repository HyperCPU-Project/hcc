#include "driver/driver.hpp"
#include "parser.tab.hpp"

namespace hcc {
  Driver::Driver() {
  }

  Driver::~Driver() = default;

  void Driver::set_root(std::unique_ptr<AstNode> node) {
    root = std::move(node);
  }
} // namespace hcc
