%option reentrant bison-bridge
%option noyywrap
%option nounput
%option extra-type="hcc::Parser*"

%{
#include "parser.hpp"
#include <string>

typedef void* yyscan_t;

#define PARSER (yyget_extra(yyscanner))
%}

%%

[ \t]           ;
[\n]            { PARSER->line_num++; }
"return"        { return RETURN; }
"asm"        { return ASM; }
[0-9]+          { yylval->number = std::stoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* {
	yylval->string = new std::string(yytext);
	return IDENTIFIER;
}
\"([^\\\"\n]|\\.)*\" {
	yylval->string = new std::string(yytext + 1, yytext + yyleng - 1);
  return STRING_LITERAL;
}
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
"&"             { return AMPERSAND; }
","             { return COMMA; }
.               { printf("[hcc] unknown token: %s\n", yytext); }

%%
