#include <ir/ir.hpp>

using namespace hcc;

void IR::optimizeStackReserve([[maybe_unused]] HCC* hcc) {
  size_t insert_index = 0;
  size_t bytes = 0;
  std::vector<std::pair<size_t, size_t>> inserts;

  for (size_t i = 0; i < ir.size(); i++) {
    IrOpcode& op = ir[i];
    if (op.type == IrOpcode::IR_FUNCDEF && insert_index == 0) {
      insert_index = i + 1;
      bytes = 0;
    } else if ((op.type == IrOpcode::IR_FUNCDEF || op.type == IrOpcode::IR_END) && insert_index > 0) {
      inserts.insert(inserts.begin(), std::make_pair(insert_index, (size_t)bytes));
      insert_index = i + 1;
      bytes = 0;
      if (op.type == IrOpcode::IR_END)
        break;
    } else if (op.type == IrOpcode::IR_ALLOCA && i != 0) {
      bytes += op.alloca.md.size;
    }
  }

  for (auto pair : inserts) {
    IrOpcode res;
    res.type = IrOpcode::IR_RESERVE;
    res.reserve.bytes = pair.second;
    ir.insert(ir.begin() + pair.first, res);
  }
}
