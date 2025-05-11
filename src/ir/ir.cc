#include <backend/backend.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

IR::IR() = default;

void IR::add_line() {
	IrOpcode op;
	op.type = IrOpcode::IR_LINE;
	ir.push_back(op);
}

void IR::add(IrOpcode op) {
	ir.push_back(op);
}

IrOpcode IR::next() {
	if (index >= ir.size()) {
		IrOpcode op;
		op.type = IrOpcode::IR_END;
		return op;
	}
	return ir[index++];
}

IrOpcode IR::peek(unsigned long count) {
	unsigned long index_old = index;
	IrOpcode op;

	op.type = IrOpcode::IR_NULL;
	for (unsigned long i = 0; i < count; i++) {
		op = next();
	}

	index = index_old;

	return op;
}

void IR::optimize_dce_unused([[maybe_unused]] HCC* hcc) {
	std::vector<std::string> used_vars;
	for (size_t passes = passes_for_each_optimization; passes > 0; passes--) {
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
				for (size_t j = i; j >= 0; j--) {
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

bool IR::opcode_affects_stack(IrOpcode op) {
	switch (op.type) {
	case IrOpcode::IR_ALLOCA:
		return true;
	case IrOpcode::IR_CSV:
		return true;
	default:
		return false;
	}
}

void IR::performStaticOptimizations(HCC* hcc) {
	if (hcc->optimizations.HasFlag(HCC::OPT_DCE_UNUSED)) {
		optimize_dce_unused(hcc);
	}
	if (hcc->optimizations.HasFlag(HCC::OPT_STACK_SETUP)) {
		optimize_stack_setup(hcc);
	}
}

bool IR::compile(HCC* hcc) {
	IrOpcode currentFuncdefOp;
	for (;;) {
		IrOpcode op = next();
		if (op.type == IrOpcode::IR_END)
			break;

		switch (op.type) {
		case IrOpcode::IR_NULL:
			hcc_compile_error = "IR_NULL opcode encountered";
			return false;
			break;
		case IrOpcode::IR_FUNCDEF:
			if (peek().type == IrOpcode::IR_RET && hcc->optimizations.HasFlag(HCC::Optimizations::OPT_ONERET)) { // function with no body that instantly returns
				hcc->backend->emit_label(op.funcdef.name);
				hcc->backend->emit_single_ret();
				next();
			} else {
				if (op.funcdef.need_stack) {
					hcc->backend->reset_reg_index();
					hcc->backend->emit_function_prologue(op.funcdef.name);
				} else {
					hcc->backend->emit_label(op.funcdef.name);
				}
				currentFuncdefOp = op;
			}
			break;
		case IrOpcode::IR_CREG: {
			auto value = std::unique_ptr<Value>(Value::createAsRegister(hcc, op.creg.value, op.creg.reg_name));
			hcc->values.push(std::move(value));
		} break;
		case IrOpcode::IR_CCTV: {
			auto value = std::unique_ptr<Value>(Value::createAsCompileTimeValue(hcc, op.cctv.value));
			hcc->values.push(std::move(value));
		} break;
		case IrOpcode::IR_CSV: {
			auto value = std::unique_ptr<Value>(Value::createAsStackVar(hcc, op.csv.md));
			hcc->values.push(std::move(value));
		} break;
		case IrOpcode::IR_RET: {
			if (!hcc->values.empty()) {
				auto value_raw = std::move(hcc->values.top());
				hcc->values.pop();

				auto value = value_raw->use(hcc);

				if (value->reg_name != hcc->backend->abi.return_register) {
					hcc->backend->emit_move(hcc->backend->abi.return_register, value->reg_name);
				}

				if (value != value_raw.get())
					delete value;
			}

			if (currentFuncdefOp.funcdef.need_stack)
				hcc->backend->emit_function_epilogue();
			else
				hcc->backend->emit_single_ret();

			while (!hcc->values.empty())
				hcc->values.pop();
		} break;
		case IrOpcode::IR_ALLOCA: {
			std::unique_ptr<Value> value(Value::createAsStackVar(hcc, op.alloca.md));
			hcc->current_function.variables[op.alloca.name] = std::move(value);
		} break;
		case IrOpcode::IR_ADD: {
			auto RHS = std::move(hcc->values.top());
			hcc->values.pop();
			auto LHS = std::move(hcc->values.top());
			hcc->values.pop();

			LHS->add(hcc, RHS.get());

			hcc->values.push(std::move(LHS));
		} break;
		case IrOpcode::IR_SUB: {
			auto RHS = std::move(hcc->values.top());
			hcc->values.pop();
			auto LHS = std::move(hcc->values.top());
			hcc->values.pop();

			LHS->sub(hcc, RHS.get());

			hcc->values.push(std::move(LHS));
		} break;
		case IrOpcode::IR_MUL: {
			auto RHS = std::move(hcc->values.top());
			hcc->values.pop();
			auto LHS = std::move(hcc->values.top());
			hcc->values.pop();

			LHS->mul(hcc, RHS.get());

			hcc->values.push(std::move(LHS));
		} break;
		case IrOpcode::IR_DIV: {
			auto RHS = std::move(hcc->values.top());
			hcc->values.pop();
			auto LHS = std::move(hcc->values.top());
			hcc->values.pop();

			LHS->div(hcc, RHS.get());

			hcc->values.push(std::move(LHS));
		} break;
		case IrOpcode::IR_ASSIGN: {
			if (!hcc->current_function.variables.contains(op.assign.name)) {
				hcc_compile_error = fmt::sprintf("undefined variable %s", op.assign.name);
				return false;
			}

			auto& expr_var = hcc->current_function.variables[op.assign.name];

			auto expr_value = std::move(hcc->values.top());
			hcc->values.pop();

			expr_var->setto(hcc, expr_value.get());
		} break;
		case IrOpcode::IR_ASM:
			hcc->backend->output += op.asm_.code + "\n";
			break;
		case IrOpcode::IR_VARREF: {
			if (!hcc->current_function.variables.contains(op.varref.name)) {
				hcc_compile_error = fmt::sprintf("undefined variable %s", op.varref.name);
				return false;
			}

			std::unique_ptr<Value> out(hcc->current_function.variables[op.varref.name]->doCondLod(hcc));

			hcc->values.push(std::move(out));
		} break;
		case IrOpcode::IR_ADDROF: {
			if (!hcc->current_function.variables.contains(op.addrof.name)) {
				hcc_compile_error = fmt::sprintf("undefined variable %s", op.addrof.name);
				return false;
			}

			auto out = std::unique_ptr<Value>(new Value());
			out->reg_name = hcc->backend->emit_loadaddr_from_stack(hcc->current_function.variables[op.addrof.name]->var_stack_align);

			hcc->values.push(std::move(out));
		} break;
		case IrOpcode::IR_LINE:
			break;
		default:
			break;
		}
	}
	return true;
}
