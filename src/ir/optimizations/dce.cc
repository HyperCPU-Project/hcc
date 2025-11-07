#include <ir/ir.hpp>

using namespace hcc;

void IR::optimizeDceUnused([[maybe_unused]] HCC* hcc) {
  std::vector<std::string> used_vars;
  for (size_t passes = passesForEachOptimization; passes > 0; passes--) {
    std::string var = "";
    std::vector<size_t> remove_indexes;
    for (size_t i = 0; i < ir.size(); i++) {
      IrOpcode& op = ir[i];

      if (op.type == IrOpcode::IR_ALLOCA && var.empty()) { // found a variable allocation, perform a unused DCE pass on it
        // detect if a variable has been confirmed used in the previous pass, otherwise try to optimize it
        if (std::find(used_vars.begin(), used_vars.end(), op.alloca.name) == used_vars.end()) {
          var = op.alloca.name;
          remove_indexes.push_back(i);
        }
      } else if (op.type == IrOpcode::IR_VARREF && op.varref.name == var) {
        used_vars.push_back(var);
        remove_indexes.clear();
        var.clear();
        break;
      } else if (op.type == IrOpcode::IR_ASSIGN && op.assign.name == var) {
        // we still optimizin' it since the variable can be assigned, but not used
        // in cases where it would be used in the future, IR_VARREF will discard the removal of this opcode
        for (size_t j = i;; j--) {
          remove_indexes.push_back(j);
          if (ir[j].type == IrOpcode::IR_LINE) {
            break;
          }
        }
      }
    }

    std::sort(remove_indexes.begin(), remove_indexes.end(), std::greater<>());

    if (!var.empty()) {
      for (size_t index : remove_indexes) {
        ir.erase(ir.begin() + index);
      }
      passes++;
    }
  }
}
