#include <dep_pch.hpp>
#include <ir/ir.hpp>

using namespace hcc;

void IR::OptimizeStackSetup([[maybe_unused]] HCC* hcc) {
  IrOpcode* currentFunc = nullptr;
  for (size_t i = 0; i < ir.size(); i++) {
    IrOpcode& op = ir[i];
    if (op.type == IrOpcode::IR_FUNCDEF) {
      currentFunc = &op;
      op.funcdef.need_stack = false;
      if (!op.funcdef.arg_names.empty()) {
        op.funcdef.need_stack = true;
      }
    } else if (OpcodeAffectsStack(op)) {
      currentFunc->funcdef.need_stack = true;
    }
  }
}
