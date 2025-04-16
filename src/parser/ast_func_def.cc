#include <backend/backend.hpp>
#include <hcc.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <value.hpp>

using namespace hcc;

AstFuncDef* AstFuncDef::create(Parser* parser) {
	AstFuncDef* me = new AstFuncDef();

	Lexer::Token type = parser->currentToken;
	Lexer::Token name = parser->lexer.next();

	if (name.type != Lexer::TokenType::IDENTIFIER) {
		fmt::print("[hcc] [%d] name should be an identifier", name.line);
		delete me;
		return nullptr;
	}

	me->name = std::get<std::string>(name.value);
	me->return_type = *parser->backend->getTypeFromName(std::get<std::string>(type.value));

	// Parse function parameters
	Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::LPAREN) {
		fmt::print("[hcc] [{}] expected '(' after function name\n", next.line);
		delete me;
		return nullptr;
	}

	next = parser->lexer.next();
	if (next.type != Lexer::TokenType::RPAREN) {
		fmt::print("[hcc] [{}] expected ')' after function parameters\n", next.line);
		delete me;
		return nullptr;
	}

	// Parse function body
	if (!parser->doBlock(me)) {
		delete me;
		return nullptr;
	}

	return me;
}

void AstFuncDef::assemble(HCC* hcc) {
	hcc->current_function.name = this->name;
	hcc->current_function.return_type = return_type;
	hcc->current_function.variables.clear();
	hcc->current_uninitialized_variables.clear();

	hcc->assembly_output += this->name + ":\n";
	hcc->assembly_output += hcc->backend->emit_function_prologue() + "\n";

	AstNode::assemble(hcc);

	for (VariableMetadata* var : hcc->current_uninitialized_variables) {
		fmt::print("[hcc] [{}] warning: uninitialized variable {}\n", var->declared_at, var->name);
	}

	hcc->current_function.name = "";
	hcc->current_function.variables.clear();
	hcc->current_uninitialized_variables.clear();
}

void AstFuncDef::print() {
	fmt::printf("%s %s {\n", return_type.name, name);
	AstNode::print();
	fmt::printf("}\n", name);
}