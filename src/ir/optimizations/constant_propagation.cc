#include <ir/ir.hpp>

using namespace hcc;

void IR::optimize_constant_propagation([[maybe_unused]] HCC* hcc) {
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
