%{
#include <iostream>
#include <string>
#include <vector>
#include <ast/ast.hpp>

extern int yylex();
extern int line_num;
void yyerror(const char* s);

hcc::AstRootNode* root = nullptr;
%}

%code requires {
#include <ast/ast.hpp>
}

%union {
	int number;
	std::string *string;
	hcc::AstNode* node;
	hcc::AstNode* stmt;

	std::vector<hcc::AstNode*>* top_stmt_list;

	std::vector<hcc::AstNode*>* stmt_list;
	std::vector<hcc::AstNode*>* func_list;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> STRING_LITERAL
%token RETURN ASM
%token ASSIGN PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON

%type <node> program function_definition
%type <func_list> function_definitions
%type <node> expression term factor
%type <node> statement declaration assignment return_statement
%type <stmt_list> statement_list
%type <node> topstatement
%type <top_stmt_list> topstatements
%type <node> asm_statement

%left PLUS MINUS
%left MULTIPLY DIVIDE

%%

program:
	topstatements {
		root = new hcc::AstRootNode();
		for (const auto& func : *$1) {
			root->children.push_back(func);
		}
		delete $1;
	}
	;

function_definitions:
	function_definition function_definitions {
		auto* fn = new std::vector<hcc::AstNode*>();
		fn->push_back($1);
		fn->insert(fn->end(), $2->begin(), $2->end());
		delete $2;
		$$ = fn;
	}
	| {
		$$ = new std::vector<hcc::AstNode*>();
	}
	;

function_definition:
	IDENTIFIER IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE {
		auto* func = new hcc::AstFuncDef();
		func->name = *$2;
		for (const auto& stmt : *$6) {
			func->children.push_back(stmt);
		}
		delete $2;
		delete $6;
		delete $1;
		$$ = func;
	}
	;

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
	}
	| topstatements topstatement {
		$1->push_back($2);
		$$ = $1;
	}
	;

topstatement:
	function_definition
	| asm_statement
	;

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
	;

declaration:
	IDENTIFIER IDENTIFIER SEMICOLON {
		auto* decl = new hcc::AstVarDeclare();
		decl->name = *$2;
		decl->type = *$1;
		delete $2;
		delete $1;
		$$ = decl;
	}
	;

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
	| LPAREN expression RPAREN {
		$$ = $2;
	}
	;

%%

void yyerror(const char* s) {
	std::cerr << "Error at line " << line_num << ": " << s << std::endl;
}
