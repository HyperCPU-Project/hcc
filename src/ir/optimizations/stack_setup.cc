#include <ir/ir.hpp>

using namespace hcc;

void IR::optimizeStackSetup([[maybe_unused]] HCC* hcc) {
  IrOpcode* currentFunc = nullptr;
  for (size_t i = 0; i < ir.size(); i++) {
    IrOpcode& op = ir[i];
    if (op.type == IrOpcode::IR_FUNCDEF) {
      currentFunc = &op;
    } else if (opcodeAffectsStack(op)) {
      currentFunc->funcdef.needStack = true;
    }
  }
}
