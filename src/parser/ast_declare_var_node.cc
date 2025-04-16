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

	me->name = std::get<std::string>(name_tok.value);
	me->variable.declared_at = name_tok.line;

	return me;
}

void AstDeclareVarNode::assemble(HCC* hcc) {
	variable.stack_align = hcc->current_function.stack_space + 4;
	variable.type = type;
	variable.name = name;

	hcc->assembly_output += hcc->backend->emit_reserve_stack_space(type.size);

	hcc->current_function.variables[name] = variable;
	hcc->current_uninitialized_variables.push_back(&hcc->current_function.variables[name]);
}

void AstDeclareVarNode::print() {
	fmt::print("{} {};\n", type.name, name);
	AstNode::print();
}