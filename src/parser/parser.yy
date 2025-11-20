%language "c++"
%require "3.8"
%define api.namespace {hcc}
%define api.parser.class {Parser}
%define api.token.constructor
%define api.value.type variant
%define parse.error verbose
%locations

%code requires {
#include <memory>
#include "ast/ast.hpp"
#include "driver/driver.hpp"
#include <dep_pch.hpp>
namespace hcc { class Driver; }
}

%parse-param { hcc::Driver& driver }
%lex-param { hcc::Driver& driver }

%code {
	namespace hcc {
		Parser::symbol_type yylex(Driver& driver);
	}
}

%token END 0 "end of file"
%token <long> NUMBER
%token <std::string> IDENTIFIER
%token <std::string> STRING_LITERAL
%token LPAREN RPAREN LSQUIRLY RSQUIRLY SEMICOLON COMMA
%token RETURN INT CHAR

%type <std::unique_ptr<hcc::AstNode>> topstatement function_definition statement declaration
%type <std::vector<std::unique_ptr<hcc::AstNode>>> topstatements block statements
%type <std::vector<std::string>> declaration_names

%%

program:
	topstatements {
		auto root = std::make_unique<hcc::AstRootNode>();
		root->children = std::move($1);
		driver.set_root(std::move(root));
	} | {
		auto root = std::make_unique<hcc::AstRootNode>();
		driver.set_root(std::move(root));
	}
	;

topstatements:
	topstatement {
		$$.clear();
		$$.push_back(std::move($1));
	} | topstatements topstatement {
		$1.push_back(std::move($2));
		$$ = std::move($1);
	}
	;

topstatement:
	function_definition {
		$$ = std::move($1);
	}
	;

type:
	INT
	| CHAR
	;

function_definition:
	IDENTIFIER IDENTIFIER LPAREN RPAREN block {
		auto func = std::make_unique<hcc::AstFuncDef>();
		func->name = $2;
		func->children = std::move($5);
		$$ = std::move(func);
	}
	;

declaration_names:
	IDENTIFIER {
		$$.clear();
		$$.push_back($1);
	}
	| declaration_names COMMA IDENTIFIER {
		$1.push_back($3);
		$$ = $1;
	}

declaration:
	IDENTIFIER declaration_names SEMICOLON {
		auto decl = std::make_unique<hcc::AstVarDeclare>();
		decl->names = $2;
		decl->type = $1;
		$$ = std::move(decl);
	}

statement:
	declaration {
		$$ = std::move($1);
	}
	;

statements:
	statement {
		$$.clear();
		$$.push_back(std::move($1));
	}
	| statements statement {
		$1.push_back(std::move($2));
		$$ = std::move($1);
	}
	;

block:
	LSQUIRLY RSQUIRLY {
		$$.clear();
	} | LSQUIRLY statements RSQUIRLY {
		$$ = std::move($2);
	}

%%

namespace hcc {
	Parser::symbol_type yylex(Driver& driver){
		static std::size_t count = 0;
		Parser::location_type loc (nullptr, 0, 0);
		if(count >= driver.tokens.size()) return Parser::make_END(loc);
		Token token = driver.tokens[count];
		++count;
		switch(token.type){
			case TokenType::Number:
				return Parser::make_NUMBER(std::get<long>(token.value.value()), loc);
			case TokenType::Identifier:
				return Parser::make_IDENTIFIER(std::get<std::string>(token.value.value()), loc);
			case TokenType::StringLiteral:
				return Parser::make_STRING_LITERAL(std::get<std::string>(token.value.value()), loc);
			case TokenType::Lparen:
				return Parser::make_LPAREN(loc);
			case TokenType::Rparen:
				return Parser::make_RPAREN(loc);
			case TokenType::Lsquirly:
				return Parser::make_LSQUIRLY(loc);
			case TokenType::Rsquirly:
				return Parser::make_RSQUIRLY(loc);
			case TokenType::Semicolon:
				return Parser::make_SEMICOLON(loc);
			case TokenType::Comma:
				return Parser::make_COMMA(loc);
			case TokenType::Return:
				return Parser::make_RETURN(loc);
			default:
				std::abort();
		}
	}

	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		fmt::println("{}", msg);
		std::abort();
	}
} // namespace hcc
