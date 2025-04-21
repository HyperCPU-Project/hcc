#include <backend/hypercpu/hypercpu_backend.hpp>

using namespace hcc;

HyperCPUBackend::HyperCPUBackend() {
	reg_index = 0;
	types["void"] = TypeMetadata{"void", 0};
	types["int"] = TypeMetadata{"int", 4};
	types["long"] = TypeMetadata{"long", 8}; // size of 4 is intentional here
	abi.return_register = "x0";
	for (int i = 2; i <= 7; i++) {
		abi.args_registers.push_back(fmt::format("x{}", i));
	}
}

uint64_t HyperCPUBackend::increment_reg_index() {
	uint64_t res = ++reg_index;
	if (reg_index > 7) {
		reg_index = 0;
	}
	return res;
}

void HyperCPUBackend::emit_function_prologue(std::string name) {
	output += "// emit_function_prologue\n";
	if (name == "main")
		output += fmt::sprintf(".attr(entry) %s:\n", name);
	else
		output += fmt::sprintf("%s:\n", name);
	output += fmt::sprintf("push xbp;\nmov xbp, xsp;\n");
}

void HyperCPUBackend::emit_function_epilogue() {
	output += "// emit_function_epilogue\n";
	output += fmt::sprintf("mov xsp, xbp;\npop xbp;\npop xip;\n");
}

std::string HyperCPUBackend::emit_mov_const(uint64_t value, std::string reg_name) {
	output += "// emit_mov_const\n";
	if (reg_name == "") {
		reg_name = fmt::format("x{}", increment_reg_index());
	}

	output += fmt::sprintf("mov %s, 0s%ld;\n", reg_name, value);

	return reg_name;
}

void HyperCPUBackend::emit_add(std::string ROUT, std::string RLHS, std::string RRHS) {
	output += "// emit_add\n";
	if (ROUT != RLHS) {
		output += fmt::sprintf("add %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	output += fmt::sprintf("add %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) {
	output += "// emit_sub\n";
	if (ROUT != RLHS) {
		output += fmt::sprintf("sub %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	output += fmt::sprintf("sub %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) {
	output += "// emit_mul\n";
	if (ROUT != RLHS) {
		output += fmt::sprintf("mul %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	output += fmt::sprintf("mul %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emit_div(std::string ROUT, std::string RLHS, std::string RRHS) {
	output += "// emit_div\n";
	if (ROUT != RLHS) {
		output += fmt::sprintf("div %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	output += fmt::sprintf("div %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emit_move(std::string rdest, std::string rsrc) {
	if (rdest != rsrc) {
		output += "// emit_move\n";
		output += fmt::sprintf("mov %s, %s;\n", rdest, rsrc);
	}
}

void HyperCPUBackend::emit_reserve_stack_space(uint64_t size) {
	output += "// emit_reserve_stack_space\n";
	output += fmt::sprintf("sub xsp, 0u%ld;\n", size);
}

std::string HyperCPUBackend::emit_load_from_stack(uint64_t align, std::string reg) {
	output += "// emit_load_from_stack\n";
	if (reg.empty()) {
		reg = "x" + std::to_string(increment_reg_index());
		while (reg == "x0" || reg == "x1")
			reg = "x" + std::to_string(increment_reg_index());
	}
	output += fmt::sprintf("mov %s, b64 ptr [xbp+0x%016llx];\n", reg, ((uint64_t)((-align) - 1)));
	return reg;
}

void HyperCPUBackend::emit_store_from_stack(uint64_t align, std::string rsrc) {
	output += "// emit_store_from_stack\n";
	output += fmt::sprintf("mov b64 ptr [xbp+0x%016llx], %s\n", ((uint64_t)((-align) - 1)), rsrc);
}

std::string HyperCPUBackend::emit_loadaddr_from_stack(uint64_t align, std::string reg) {
	output += "// emit_loadaddr_from_stack\n";
	if (reg.empty())
		reg = std::to_string(increment_reg_index());
	if (reg == "r0")
		reg = std::to_string(increment_reg_index());
	reg = "r" + reg;

	output += fmt::sprintf("mov %s, xbp;\n", reg);
	output += fmt::sprintf("sub %s, %d;\n", reg, align);
	return reg;
}

void HyperCPUBackend::emit_call(std::string name) {
	output += "// emit_call\n";
	output += fmt::sprintf("call %s\n", name);
}

void HyperCPUBackend::emit_push(std::string reg) {
	output += "// emit_push\n";
	output += fmt::sprintf("push %s\n", reg);
}

void HyperCPUBackend::emit_pop(std::string reg) {
	output += "// emit_pop\n";
	output += fmt::sprintf("pop %s\n", reg);
}
