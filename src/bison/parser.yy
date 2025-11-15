%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }
%parse-param { hcc::Parser *ctx }

%{
#include <iostream>
#include <string>
#include <vector>
#include <ast/ast.hpp>
#include <parser.tab.hpp>
#include <dep_pch.hpp>

typedef void* yyscan_t;
int yylex(YYSTYPE *yylval_param, yyscan_t scanner);
void yyerror(yyscan_t scanner,hcc::Parser *ctx, const char *msg);

std::string hcc_parse_error = "";

#define PARSER (yyget_extra(scanner))
%}

%code requires {
#include <ast/ast.hpp>

struct ParserArgData {
	std::string name;
	std::string type;
};

namespace hcc {
	struct Parser {
		unsigned long line_num = 1;
		hcc::AstRootNode* root = nullptr;
	};
}

hcc::Parser* yyget_extra(void*);

}

%union {
	int number;
	std::string *string;
	hcc::AstNode* node;
	hcc::AstNode* stmt;

	std::vector<hcc::AstNode*>* top_stmt_list;

	std::vector<hcc::AstNode*>* stmt_list;
	std::vector<hcc::AstNode*>* func_list;

	std::map<std::string, std::string>* arg_list;
	std::vector<hcc::AstNode*>* call_arg_list;

	std::vector<std::string>* string_vec;

	ParserArgData *arg;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> STRING_LITERAL
%token RETURN ASM
%token ASSIGN PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON AMPERSAND COMMA

%type <node> program function_definition
%type <node> expression term factor
%type <node> statement declaration assignment return_statement
%type <stmt_list> statement_list block
%type <node> topstatement
%type <top_stmt_list> topstatements
%type <node> asm_statement
%type <arg_list> arg_list
%type <arg> arg
%type <call_arg_list> call_arg_list
%type <node> call_arg
%type <node> fncall
%type <string_vec> declaration_names

%left PLUS MINUS
%left MULTIPLY DIVIDE

%%

program:
	topstatements {
		PARSER->root = new hcc::AstRootNode();
		for (const auto& func : *$1) {
			PARSER->root->children.push_back(func);
		}
		delete $1;
	}
	;

function_definition:
	IDENTIFIER IDENTIFIER LPAREN arg_list block {
		auto* func = new hcc::AstFuncDef();
		func->name = *$2;
		func->args = *$4;
		for (const auto& stmt : *$5) {
			func->children.push_back(stmt);
		}
		delete $4;
		delete $2;
		delete $5;
		delete $1;
		$$ = func;
	}
	;

arg_list:
  arg {
    $$ = new std::map<std::string, std::string>();
    (*$$)[$1->name] = $1->type;
    delete $1;
  }
  | arg_list arg {
    (*$$)[$2->name] = $2->type;
    $$ = $1;
    delete $2;
  }
  | RPAREN {
    $$ = new std::map<std::string, std::string>();
  }
  ;

arg:
  IDENTIFIER IDENTIFIER COMMA {
    $$ = new ParserArgData();
    $$->name = *$2;
    $$->type = *$1;
    delete $1;
    delete $2;
  }
  | IDENTIFIER IDENTIFIER RPAREN {
    $$ = new ParserArgData();
    $$->name = *$2;
    $$->type = *$1;
    delete $1;
    delete $2;
  }

asm_statement:
	ASM LPAREN STRING_LITERAL RPAREN SEMICOLON {
		auto def = new hcc::AstAsm();
		def->code = *$3;
		delete $3;
		$$ = def;
	}
	;

topstatements:
	topstatement {
		$$ = new std::vector<hcc::AstNode*>();
		$$->push_back($1);
	} | topstatements topstatement {
		$1->push_back($2);
		$$ = $1;
	}
	;

topstatement:
	function_definition
	| asm_statement
	;

block:
	LBRACE statement_list RBRACE {
		$$ = $2;
	}
	| LBRACE RBRACE {
		$$ = new std::vector<hcc::AstNode*>();
	}

statement_list:
	statement {
		$$ = new std::vector<hcc::AstNode*>();
		$$->push_back($1);
	}
	| statement_list statement {
		$1->push_back($2);
		$$ = $1;
	}
	;

statement:
	declaration
	| assignment
	| return_statement
	| asm_statement
	| fncall SEMICOLON
	;

declaration:
	IDENTIFIER declaration_names SEMICOLON {
		auto* decl = new hcc::AstVarDeclare();
		decl->names = *$2;
		decl->type = *$1;
		delete $2;
		delete $1;
		$$ = decl;
	}
	;

declaration_names:
	IDENTIFIER {
		auto* names = new std::vector<std::string>();
		names->push_back(*$1);
		delete $1;
		$$ = names;
	}
	| declaration_names COMMA IDENTIFIER {
		$1->push_back(*$3);
		delete $3;
		$$ = $1;
	}

fncall:
  IDENTIFIER LPAREN call_arg_list {
    auto node = new hcc::AstFuncCall();
    node->name = *$1;
    node->args = *$3;
    delete $3;
    delete $1;
    $$ = node;
  }

call_arg_list:
  call_arg {
    $$ = new std::vector<hcc::AstNode*>();
    $$->push_back($1);
  } | call_arg_list call_arg {
    $1->push_back($2);
    $$ = $1;
  } | RPAREN {
    $$ = new std::vector<hcc::AstNode*>();
  }

call_arg:
  expression COMMA {
    $$ = $1;
  }
  | expression RPAREN {
    $$ = $1;
  }

assignment:
	IDENTIFIER ASSIGN expression SEMICOLON {
		auto* assign = new hcc::AstVarAssign();
		assign->name = *$1;
		assign->expr = $3;
		delete $1;
		$$ = assign;
	}
	;

return_statement:
	RETURN expression SEMICOLON {
		auto* ret = new hcc::AstReturn();
		ret->expr = $2;
		$$ = ret;
	}
	| RETURN SEMICOLON {
		auto* ret = new hcc::AstReturn();
		$$ = ret;
	}
	;

expression:
	term
	| expression PLUS term {
		auto* op = new hcc::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "add";
		$$ = op;
	}
	| expression MINUS term {
		auto* op = new hcc::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "sub";
		$$ = op;
	}
	;

term:
	factor
	| term MULTIPLY factor {
		auto* op = new hcc::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "mul";
		$$ = op;
	}
	| term DIVIDE factor {
		auto* op = new hcc::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "div";
		$$ = op;
	}
	;

factor:
	NUMBER {
		$$ = new hcc::AstNumber($1);
	}
	| IDENTIFIER {
		auto* var = new hcc::AstVarRef();
		var->name = *$1;
		delete $1;
		$$ = var;
	}
	| AMPERSAND IDENTIFIER {
		auto* ast = new hcc::AstAddrof();
		ast->name = *$2;
		delete $2;
		$$ = ast;
	}
	| LPAREN expression RPAREN {
		$$ = $2;
	}
	| fncall
	;

%%

void yyerror([[maybe_unused]] yyscan_t scanner,hcc::Parser *ctx, const char *s) {
	hcc_parse_error = fmt::format("error at line {}: {}", ctx->line_num, s);
}
