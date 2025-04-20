#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstVarRef::print(int indent) const {
	printIndent(indent);
	std::cout << name << std::endl;
}

bool AstVarRef::compile(HCC* hcc) {
	if (!hcc->current_function.variables.contains(name)) {
		hcc_compile_error = fmt::sprintf("undefined variable %s", name);
		return false;
	}

	std::unique_ptr<Value> out(hcc->current_function.variables[name]->doCondLod(hcc));

	hcc->values.push(std::move(out));

	hcc->backend->output += "\n";

	return true;
}
