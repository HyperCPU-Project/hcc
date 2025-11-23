#include "ir/ir.hpp"
#include <ast/ast.hpp>
#include <cassert>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstBinaryOp::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstBinaryOp" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "op: " << op << std::endl;
  PrintIndent(indent + 1);
  std::cout << "left:" << std::endl;
  left->Print(indent + 2);
  PrintIndent(indent + 1);
  std::cout << "right:" << std::endl;
  right->Print(indent + 2);
}

AstBinaryOp::~AstBinaryOp() {
}

static size_t GetNodeLength(AstNode* node) {
  if ([[maybe_unused]] auto num = dynamic_cast<AstNumber*>(node)) {
    return 1;
  } else if (auto funccall = dynamic_cast<AstFuncCall*>(node)) {
    size_t len = 0;
    for (auto& child : funccall->children) {
      len += GetNodeLength(child.get());
    }
    return len;
  } else if (auto binop = dynamic_cast<AstBinaryOp*>(node)) {
    size_t len = 0;
    len += GetNodeLength(binop->left.get());
    len += GetNodeLength(binop->right.get());
    return len;
  }
  assert(false && "GetNodeLength: unhandled path");
}

bool AstBinaryOp::Compile(HCC* hcc) {
  IrOpcode ir_op;
  size_t len_left = GetNodeLength(left.get());
  size_t len_right = GetNodeLength(right.get());
  fmt::println("{} {}", len_left, len_right);
  if (len_right > len_left) {
    ir_op.stack_pop_mode = StackPopMode::LHS_FIRST;
    if (!right->Compile(hcc))
      return false;
    if (!left->Compile(hcc))
      return false;
  } else {
    ir_op.stack_pop_mode = StackPopMode::RHS_FIRST;
    if (!left->Compile(hcc))
      return false;
    if (!right->Compile(hcc))
      return false;
  }

  if (op == "add") {
    ir_op.type = IrOpcode::IR_ADD;
  } else if (op == "sub") {
    ir_op.type = IrOpcode::IR_SUB;
  } else if (op == "mul") {
    ir_op.type = IrOpcode::IR_MUL;
  } else if (op == "div") {
    ir_op.type = IrOpcode::IR_DIV;
  }

  hcc->ir.Add(ir_op);

  return true;
}
