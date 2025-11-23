#include <ir/ir.hpp>

using namespace hcc;

void IR::OptimizeStackSetup([[maybe_unused]] HCC* hcc) {
  IrOpcode* currentFunc = nullptr;
  for (size_t i = 0; i < ir.size(); i++) {
    IrOpcode& op = ir[i];
    if (op.type == IrOpcode::IR_FUNCDEF) {
      currentFunc = &op;
    } else if (OpcodeAffectsStack(op)) {
      currentFunc->funcdef.need_stack = true;
    }
  }
}
