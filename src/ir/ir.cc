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

void IR::add_reset() {
	IrOpcode op;
	op.type = IrOpcode::IR_RESET;
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

void IR::optimize_stack_reserve([[maybe_unused]] HCC* hcc) {
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

void IR::optimize_constant_propagation(HCC* hcc) {
	struct ConstantVarInfo {
		std::string name;
		bool is_potentially_optimizable = true;
		std::vector<IrOpcode> const_value_definition_opcodes;
		size_t alloca_idx = static_cast<size_t>(-1);
		size_t last_assign_idx = static_cast<size_t>(-1);
		size_t const_value_def_start_idx_in_ir = static_cast<size_t>(-1);
		std::vector<size_t> varref_indices;
		int num_assignments = 0;
		bool address_taken = false;
	};
	std::map<std::string, ConstantVarInfo> variables_info;

	// phase 1: gather information about variables
	for (size_t i = 0; i < ir.size(); ++i) {
		const auto& op = ir[i];
		switch (op.type) {
		case IrOpcode::IR_ALLOCA: {
			// initialize or re-initialize var info if reparsing (though not typical for one pass)
			variables_info[op.alloca.name].name = op.alloca.name;
			variables_info[op.alloca.name].alloca_idx = i;
			variables_info[op.alloca.name].num_assignments = 0;
			variables_info[op.alloca.name].is_potentially_optimizable = true;
			variables_info[op.alloca.name].address_taken = false;
			variables_info[op.alloca.name].const_value_definition_opcodes.clear();
			variables_info[op.alloca.name].varref_indices.clear(); // Clear varrefs for fresh analysis
			variables_info[op.alloca.name].last_assign_idx = static_cast<size_t>(-1);
			variables_info[op.alloca.name].const_value_def_start_idx_in_ir = static_cast<size_t>(-1);
			break;
		}
		case IrOpcode::IR_ASSIGN: {
			auto it = variables_info.find(op.assign.name);
			if (it != variables_info.end()) {
				ConstantVarInfo& current_var_info = it->second;
				current_var_info.num_assignments++;
				current_var_info.last_assign_idx = i;

				if (!current_var_info.is_potentially_optimizable || current_var_info.num_assignments > 1) {
					current_var_info.is_potentially_optimizable = false;
					current_var_info.const_value_definition_opcodes.clear();
					continue;
				}

				std::vector<IrOpcode> current_def_ops;
				int values_needed = 1; // one value for the assignment
				size_t def_start_idx = i;
				bool possible_const_def = true;

				if (i == 0) { // cannot trace back if assign is the first op
					possible_const_def = false;
				}

				for (long j = (long)i - 1; j >= 0 && possible_const_def; --j) {
					const auto& prev_op = ir[j];

					// logic to determine how many values are on stack vs needed
					// simplified: ADD/SUB/MUL/DIV: pops 2, pushes 1. net needed before it: +1.
					// CCTV: pops 0, pushes 1. Net needed before it: -1.
					int pushed_by_prev = 0;
					int popped_by_prev = 0;

					switch (prev_op.type) {
					case IrOpcode::IR_CCTV:
						pushed_by_prev = 1;
						break;
					case IrOpcode::IR_ADD:
					case IrOpcode::IR_SUB:
					case IrOpcode::IR_MUL:
					case IrOpcode::IR_DIV:
						popped_by_prev = 2;
						pushed_by_prev = 1;
						break;
					case IrOpcode::IR_VARREF:			// per user's example, VARREF in expression makes it non-const for this pass
						pushed_by_prev = 1;					// it pushes a value
						possible_const_def = false; // TODO
						break;
					default:
						possible_const_def = false;
						break;
					}

					if (!possible_const_def)
						break;

					current_def_ops.insert(current_def_ops.begin(), prev_op);
					def_start_idx = j;

					values_needed -= pushed_by_prev;
					values_needed += popped_by_prev;

					if (values_needed == 0)
						break;
					if (values_needed < 0) { // consumed more than available from current sequence
						possible_const_def = false;
						break;
					}
				}

				if (possible_const_def && values_needed == 0) {
					current_var_info.const_value_definition_opcodes = current_def_ops;
					current_var_info.const_value_def_start_idx_in_ir = def_start_idx;
				} else {
					current_var_info.is_potentially_optimizable = false;
					current_var_info.const_value_definition_opcodes.clear();
				}
			}
			break;
		}
		case IrOpcode::IR_VARREF: {
			auto it = variables_info.find(op.varref.name);
			if (it != variables_info.end()) {
				it->second.varref_indices.push_back(i);
			}
			break;
		}
		case IrOpcode::IR_ADDROF: {
			auto it = variables_info.find(op.addrof.name);
			if (it != variables_info.end()) {
				it->second.address_taken = true;
				it->second.is_potentially_optimizable = false;
			}
			break;
		}
		default:
			break;
		}
	}

	// final check on optimizability after all info is gathered
	for (auto& entry : variables_info) {
		ConstantVarInfo& info = entry.second;
		if (!info.is_potentially_optimizable)
			continue; // already ruled out

		if (info.address_taken || info.num_assignments != 1 || info.const_value_definition_opcodes.empty() || info.alloca_idx == static_cast<size_t>(-1) || info.last_assign_idx == static_cast<size_t>(-1) ||
				info.const_value_def_start_idx_in_ir == static_cast<size_t>(-1)) {
			info.is_potentially_optimizable = false;
		}
	}

	// Phase 3: transformation
	std::vector<IrOpcode> new_ir;
	std::vector<bool> op_is_deleted(ir.size(), false);

	for (const auto& entry : variables_info) {
		const ConstantVarInfo& info = entry.second;
		if (info.is_potentially_optimizable) { // implicitly checks all conditions from above
			op_is_deleted[info.alloca_idx] = true;
			op_is_deleted[info.last_assign_idx] = true;
			for (size_t k = info.const_value_def_start_idx_in_ir; k < info.last_assign_idx; ++k) {
				op_is_deleted[k] = true;
			}
		}
	}

	for (size_t i = 0; i < ir.size(); ++i) {
		if (op_is_deleted[i]) {
			continue;
		}

		const auto& op = ir[i];
		if (op.type == IrOpcode::IR_VARREF) {
			auto it = variables_info.find(op.varref.name);
			if (it != variables_info.end() && it->second.is_potentially_optimizable) {
				for (const auto& def_op : it->second.const_value_definition_opcodes) {
					new_ir.push_back(def_op);
				}
			} else {
				new_ir.push_back(op);
			}
		} else {
			new_ir.push_back(op);
		}
	}
	ir = new_ir;
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
	{
		IrOpcode op;
		op.type = IrOpcode::IR_END;
		add(op);
	}

	if (hcc->optimizations.HasFlag(HCC::OPT_CONSTANT_PROPAGATION)) {
		optimize_constant_propagation(hcc);
	}
	if (hcc->optimizations.HasFlag(HCC::OPT_DCE)) {
		optimize_dce_unused(hcc);
	}
	if (hcc->optimizations.HasFlag(HCC::OPT_FP_OMISSION)) {
		optimize_stack_setup(hcc);
	}
	if (hcc->optimizations.HasFlag(HCC::OPT_STACK_RESERVE)) {
		optimize_stack_reserve(hcc);
	}
}

bool IR::compile(HCC* hcc) {
	/*for (IrOpcode& op : ir) {
		fmt::println("{}", (int)op.type);
	}*/
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
			if (peek().type == IrOpcode::IR_RET && hcc->optimizations.HasFlag(HCC::OPT_FUNCTION_BODY_ELIMINATION)) { // function with no body that instantly returns
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
			std::unique_ptr<Value> value(Value::createAsStackVar(hcc, op.alloca.md, false));
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
		case IrOpcode::IR_CALL: {
			hcc->backend->emit_call(op.call.name);
			auto value = std::make_unique<Value>();
			value->reg_name = hcc->backend->abi.return_register;

			hcc->values.push(std::move(value));
		} break;
		case IrOpcode::IR_LINE:
			break;
		case IrOpcode::IR_RESET:
			hcc->backend->reset_reg_index();
			break;
		case IrOpcode::IR_RESERVE:
			if (op.reserve.bytes > 0)
				hcc->backend->emit_reserve_stack_space(op.reserve.bytes);
			break;
		default:
			break;
		}
	}
	return true;
}
