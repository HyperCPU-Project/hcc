
#include <backend/backend.hpp>
#include <hcc.hpp>
#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <value.hpp>

using namespace hcc;

AstAssignVarNode* AstAssignVarNode::create(Parser* parser) {
	AstAssignVarNode* me = new AstAssignVarNode();

	me->name = parser->currentToken;
	me->nameValueString = std::get<std::string>(me->name.value);

	parser->lexer.next(); // consume =
	parser->currentToken = parser->lexer.next();

	me->expression = parser->parseExpression();

	return me;
}

bool AstAssignVarNode::assemble(HCC* hcc) {
	if (!expression->assemble(hcc))
		return false;

	if (!hcc->current_function.variables.contains(nameValueString)) {
		fmt::print("[hcc] [{}] undeclared variable {}\n", name.line, nameValueString);
		return false;
	}

	value = hcc->current_function.variables[nameValueString];

	{
		std::vector<Value*>& vec = hcc->current_uninitialized_variables;
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec.at(i)->var.name == std::get<std::string>(name.value)) {
				vec.erase(vec.begin() + i);
				break;
			}
		}
	}

	Value result = hcc->backend->values.top();
	hcc->backend->values.pop();

	return true;
}

void AstAssignVarNode::print() {
	fmt::print("(assignment to {})\n", std::get<std::string>(name.value));
	AstNode::print();
}