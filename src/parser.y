%{
#include <pch.hpp>
#include "ast.hpp"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);

AstFuncDef* root = nullptr;
%}

%union {
    int intval;
    char* id;
    AstNode* node;
    std::vector<AstNode*>* nodeList;
}

%token <intval> NUMBER
%token <id> IDENTIFIER
%token RETURN INT
%token CHAR

%type <node> expr stmt var_decl var_assign return_stmt
%type <nodeList> stmt_list

%start program

%%

program:
    INT IDENTIFIER '(' INT IDENTIFIER ',' CHAR '*' '*' IDENTIFIER
 ')' '{' stmt_list '}' {
        root = new AstFuncDef($2);
        root->body = *$12;
        delete $12;
    }
;

stmt_list:
    /* empty */ { $$ = new std::vector<AstNode*>(); }
    | stmt_list stmt { $1->push_back($2); $$ = $1; }
;

stmt:
    var_decl
    | var_assign
    | return_stmt
;

var_decl:
    INT IDENTIFIER ';' {
        $$ = new AstVarDeclare($2);
    }
;

var_assign:
    IDENTIFIER '=' expr ';' {
        $$ = new AstVarAssign($1, $3);
    }
;

return_stmt:
    RETURN expr ';' {
        $$ = new AstReturn($2);
    }
;

expr:
    expr '+' expr { $$ = new AstBinaryOp("+", $1, $3); }
    | expr '-' expr { $$ = new AstBinaryOp("-", $1, $3); }
    | expr '*' expr { $$ = new AstBinaryOp("*", $1, $3); }
    | expr '/' expr { $$ = new AstBinaryOp("/", $1, $3); }
    | '(' expr ')' { $$ = $2; }
    | NUMBER { $$ = new AstNumber($1); }
;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}
