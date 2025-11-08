#include "../ir.hpp"
using namespace hcc;

void IR::OptimizeConstantPropagation(HCC* hcc) {
  (void)hcc;
  for (size_t passes = passes_for_each_optimization; passes > 0; passes--) {
    for (size_t i = 0; i < ir.size(); i++) {
      IrOpcode& op = ir.at(i);
      if (op.type == IrOpcode::IR_CCTV) {
        // signature
        // cctv ??
        // cctv ??
        // add/sub/mul/div
        IrOpcode cctv2 = ir.at(i + 1);
        IrOpcode alu = ir.at(i + 2);
        if (cctv2.type != IrOpcode::IR_CCTV) {
          continue;
        }
        size_t num1 = op.cctv.value;
        size_t num2 = cctv2.cctv.value;
        size_t num;
        bool valid = true;
        switch (alu.type) {
        case IrOpcode::IR_ADD:
          num = num1 + num2;
          break;
        case IrOpcode::IR_SUB:
          num = num1 - num2;
          break;
        case IrOpcode::IR_MUL:
          num = num1 * num2;
          break;
        case IrOpcode::IR_DIV:
          num = num1 / num2;
          break;
        default:
          valid = false;
          break;
        }
        if (!valid)
          continue;

        op.cctv.value = num;
        ir.erase(ir.begin() + i + 2);
        ir.erase(ir.begin() + i + 1);
      }
    }
  }
}
