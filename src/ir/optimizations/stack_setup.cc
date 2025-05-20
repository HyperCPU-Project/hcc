#include <ir/ir.hpp>

using namespace hcc;

void IR::optimize_stack_setup([[maybe_unused]] HCC* hcc) {
  IrOpcode* currentFunc = nullptr;
  for (size_t i = 0; i < ir.size(); i++) {
    IrOpcode& op = ir[i];
    if (op.type == IrOpcode::IR_FUNCDEF) {
      currentFunc = &op;
    } else if (opcode_affects_stack(op)) {
      currentFunc->funcdef.need_stack = true;
    }
  }
}
