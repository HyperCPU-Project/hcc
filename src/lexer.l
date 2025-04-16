%{
#include "parser.hpp"
#include <string>

#define YY_DECL int yylex()

int line_num = 1;
%}

%%

[ \t]           ; // Ignore whitespace
[\n]            { line_num++; }
"int"           { return TYPE_INT; }
"return"        { return RETURN; }
[0-9]+          { yylval.number = std::stoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = new std::string(yytext); return IDENTIFIER; }
"="             { return ASSIGN; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMICOLON; }
.               { printf("Unknown token: %s\n", yytext); }

%%

int yywrap() {
    return 1;
} 