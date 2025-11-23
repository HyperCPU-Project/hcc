#include "driver/driver.hpp"
#include "parser.tab.hpp"

namespace hcc {
  Driver::Driver()
      : root(nullptr), tokens({}), token_i(0), error() {
  }

  Driver::~Driver() = default;

  void Driver::set_root(std::unique_ptr<AstNode> node) {
    root = std::move(node);
  }
} // namespace hcc
