%{
#include "parser.tab.h"
#include <cstring>
%}

%%

"int"       { return INT; }
"char"      { return CHAR; }
"return"    { return RETURN; }

[0-9]+      { yylval.intval = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* {
                yylval.id = strdup(yytext);
                return IDENTIFIER;
            }

"=="|"!="|"<="|">="|&&"||"
"+"|"-"|"*"|"/"|"="         { return *yytext; }
"(" | ")" | "{" | "}" | ";" | "," | "*" { return *yytext; }

[ \t\r\n]+
.            { printf("Unknown character: %s\n", yytext); }

%%

int yywrap() { return 1; }
