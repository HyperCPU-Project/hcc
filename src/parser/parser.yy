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
		static Parser::symbol_type yylex(Driver& driver);
	}
}

%token END 0 "end of file"
%token <long> NUMBER
%token <std::string> IDENTIFIER
%token <std::string> STRING_LITERAL
%token LPAREN RPAREN LSQUIRLY RSQUIRLY SEMICOLON COMMA PLUS MINUS MULTIPLY DIVIDE AMPERSAND ASSIGN
%token RETURN ASM

%type <std::unique_ptr<hcc::AstNode>> topstatement statement expression term factor
%type <std::unique_ptr<hcc::AstVarDeclare>> declaration
%type <std::unique_ptr<hcc::AstFuncDef>> function_definition 
%type <std::unique_ptr<hcc::AstReturn>> return_statement
%type <std::unique_ptr<hcc::AstVarAssign>> assignment
%type <std::unique_ptr<hcc::AstFuncCall>> fncall 
%type <std::unique_ptr<hcc::AstAsm>> asm_statement 
%type <std::vector<std::unique_ptr<hcc::AstNode>>> topstatements block statements call_arg_list
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

function_definition:
	IDENTIFIER IDENTIFIER LPAREN RPAREN block {
		$$ = std::make_unique<hcc::AstFuncDef>();
		$$->name = $2;
		$$->children = std::move($5);
	}
	;

call_arg_list:
  expression {
	$$.clear();
    $$.push_back(std::move($1));
  } | call_arg_list COMMA expression {
    $1.push_back(std::move($3));
	$$ = std::move($1);
  }

fncall:
  IDENTIFIER LPAREN call_arg_list RPAREN {
    $$ = std::make_unique<hcc::AstFuncCall>();
    $$->name = $1;
    $$->args = std::move($3);
  }

declaration_names:
	IDENTIFIER {
		$$.clear();
		$$.push_back($1);
	}
	| declaration_names COMMA IDENTIFIER {
		$1.push_back($3);
		$$ = $1;
	}
	;

declaration:
	IDENTIFIER declaration_names SEMICOLON {
		$$ = std::make_unique<hcc::AstVarDeclare>();
		$$->names = $2;
		$$->type = $1;
	}
	;

return_statement:
	RETURN expression SEMICOLON {
		$$ = std::make_unique<hcc::AstReturn>();
		$$->expr = std::move($2);
	}
	| RETURN SEMICOLON {
		$$ = std::make_unique<hcc::AstReturn>();
	}
	;

assignment:
	IDENTIFIER ASSIGN expression SEMICOLON {
		$$ = std::make_unique<hcc::AstVarAssign>();
		$$->name = $1;
		$$->expr = std::move($3);
	}
	;

asm_statement:
	ASM LPAREN STRING_LITERAL RPAREN SEMICOLON {
		$$ = std::make_unique<hcc::AstAsm>();
		$$->code = $3;
	}
	;

statement:
	declaration {
		$$ = std::move($1);
	} | return_statement {
		$$ = std::move($1);
	} | assignment {
		$$ = std::move($1);
	} | fncall {
		$$ = std::move($1);
	} | asm_statement {
		$$ = std::move($1);
	} | assignment {
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
	;

expression:
	term {
		$$ = std::move($1);
	} | expression PLUS term {
		auto node = std::make_unique<hcc::AstBinaryOp>();
		node->left = std::move($1);
		node->right = std::move($3);
		node->op = "add";
		$$ = std::move(node);
	} | expression MINUS term {
		auto node = std::make_unique<hcc::AstBinaryOp>();
		node->left = std::move($1);
		node->right = std::move($3);
		node->op = "sub";
		$$ = std::move(node);
	}
	;

term:
	factor {
		$$ = std::move($1);
	} | term MULTIPLY factor {
		auto node = std::make_unique<hcc::AstBinaryOp>();
		node->left = std::move($1);
		node->right = std::move($3);
		node->op = "mul";
		$$ = std::move(node);
	} | term DIVIDE factor {
		auto node = std::make_unique<hcc::AstBinaryOp>();
		node->left = std::move($1);
		node->right = std::move($3);
		node->op = "div";
		$$ = std::move(node);
	}
	;

factor:
	NUMBER {
		$$ = std::make_unique<hcc::AstNumber>($1);
	} | IDENTIFIER {
		auto node = std::make_unique<hcc::AstVarRef>();
		node->name = $1;
		$$ = std::move(node);
	} | AMPERSAND IDENTIFIER {
		auto node = std::make_unique<hcc::AstAddrof>();
		node->name = $2;
		$$ = std::move(node);
	} | LPAREN expression RPAREN {
		$$ = std::move($2);
	} | fncall {
		$$ = std::move($1);
	}
	;

%%

namespace hcc {
	Parser::symbol_type yylex(Driver& driver){
		Parser::location_type loc (nullptr, 0, 0);
		if(driver.token_i >= driver.tokens.size()) return Parser::make_END(loc);
		Token token = driver.tokens[driver.token_i];
		++driver.token_i;
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
			case TokenType::Add:
				return Parser::make_PLUS(loc);
			case TokenType::Sub:
				return Parser::make_MINUS(loc);
			case TokenType::Mul:
				return Parser::make_MULTIPLY(loc);
			case TokenType::Div:
				return Parser::make_DIVIDE(loc);
			case TokenType::Ampersand:
				return Parser::make_AMPERSAND(loc);
			case TokenType::Assign:
				return Parser::make_ASSIGN(loc);
			case TokenType::Return:
				return Parser::make_RETURN(loc);
			case TokenType::Asm:
				return Parser::make_ASM(loc);
			default:
				std::abort();
		}
	}

	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		driver.error = msg;
	}
} // namespace hcc
