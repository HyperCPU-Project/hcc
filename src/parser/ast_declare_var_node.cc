#include <backend/backend.hpp>
#include <hcc.hpp>
#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <value.hpp>

using namespace hcc;

AstDeclareVarNode* AstDeclareVarNode::create(Parser* parser) {
	AstDeclareVarNode* me = new AstDeclareVarNode();

	me->type = *parser->backend->getTypeFromName(std::get<std::string>(parser->currentToken.value));

	Lexer::Token name_tok = parser->lexer.next();
	if (name_tok.type != Lexer::TokenType::IDENTIFIER) {
		fmt::print("[hcc] [{}] expected an identifier as the variable name {}\n", name_tok.line, (int)name_tok.type);
		delete me;
		return nullptr;
	}

	me->value = new Value();
	me->value->var.declared_at = name_tok.line;
	me->value->var.name = std::get<std::string>(name_tok.value);

	return me;
}

bool AstDeclareVarNode::assemble(HCC* hcc) {
	value->var.stack_align = hcc->current_function.stack_space + 4;
	value->var.type = type;

	hcc->assembly_output += hcc->backend->emit_reserve_stack_space(type.size);

	hcc->current_function.variables[value->var.name] = value;
	hcc->current_uninitialized_variables.push_back(hcc->current_function.variables[value->var.name]);

	return true;
}

void AstDeclareVarNode::print() {
	fmt::print("{} {};\n", type.name, value->var.name);
	AstNode::print();
}