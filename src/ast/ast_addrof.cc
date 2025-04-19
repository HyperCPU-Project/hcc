#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAddrof::print(int indent) const {
	printIndent(indent);
	std::cout << "&" + name << std::endl;
}

bool AstAddrof::compile(HCC* hcc) {
	if (!hcc->current_function.variables.contains(name)) {
		fmt::print("[hcc] undefined variable {}\n", name);
		return false;
	}

	auto out = std::unique_ptr<Value>(new Value());
	out->reg_name = hcc->backend->emit_loadaddr_from_stack(hcc->getOutFd(), hcc->current_function.variables[name]->var_stack_align);

	hcc->values.push(std::move(out));

	fprintf(hcc->getOutFd(), "\n");

	return true;
}
