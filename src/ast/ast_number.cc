#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstNumber::print(int indent) const {
	printIndent(indent);
	std::cout << "AstNumber" << std::endl;
	printIndent(indent + 1);
	std::cout << "value: " << value << std::endl;
}

bool AstNumber::compile(HCC* hcc) {
	std::unique_ptr<Value> v(Value::createAsRegister(hcc, value));
	hcc->values.push(std::move(v));
	hcc->backend->output += "\n";
	return true;
}
