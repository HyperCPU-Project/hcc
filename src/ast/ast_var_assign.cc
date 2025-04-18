#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstVarAssign::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarAssign" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "expr:" << std::endl;
	expr->print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
	delete expr;
}

bool AstVarAssign::compile(HCC* hcc) {
	if (!expr->compile(hcc))
		return false;

	if (!hcc->current_function.variables.contains(name)) {
		fmt::print("[hcc] undefined variable {}\n", name);
		return false;
	}

	auto& expr_var = hcc->current_function.variables[name];

	auto expr_value = std::move(hcc->values.top());
	hcc->values.pop();

	expr_var->setto(hcc, expr_value.get());

	return true;
}
