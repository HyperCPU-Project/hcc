#include <backend/backend.hpp>

using namespace hcc;

void Backend::peephole_optimize() {
  // useless push pop
  for (size_t i = 0; i < emitcalls.size(); i++) {
    struct EmitCall& ec = emitcalls.at(i);
    if (ec.type == EmitCall::PUSH) {
      struct EmitCall next = emitcalls.at(i + 1);
      if (next.type == EmitCall::POP) {
        ec.move.rdest = next.pop.reg;
        ec.move.rsrc = ec.push.reg;

        ec.type = EmitCall::MOVE;

        emitcalls.erase(emitcalls.begin() + i + 1);
      }
    }
    if (ec.type == EmitCall::PUSH_IMM) {
      struct EmitCall next = emitcalls.at(i + 1);
      if (next.type == EmitCall::POP) {
        ec.mov_const.reg_name = next.pop.reg;
        ec.mov_const.value = ec.push_imm.val;

        ec.type = EmitCall::MOV_CONST;

        emitcalls.erase(emitcalls.begin() + i + 1);
      }
    }
  }

  // useless move
  for (size_t i = 0; i < emitcalls.size(); i++) {
    struct EmitCall& ec = emitcalls.at(i);
    if (ec.type == EmitCall::MOVE) {
      if (ec.move.rdest == ec.move.rsrc) {
        emitcalls.erase(emitcalls.begin() + i);
        i--;
      }
    }
  }
}
