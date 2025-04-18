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
		fmt::print("[hcc] undefined variable {}\n", name);
		return false;
	}

	std::unique_ptr<Value> out(hcc->current_function.variables[name]->doCondLod(hcc));

	hcc->values.push(std::move(out));

	return true;
}
