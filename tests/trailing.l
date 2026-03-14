%{
#include <stdio.h>
%}

%%

"abc"/"def"  { printf("Found abc followed by def (yytext=[%s])\n", yytext); }
"abc"        { printf("Found abc alone (yytext=[%s])\n", yytext); }
"def"        { printf("Found def (yytext=[%s])\n", yytext); }
.|\n         { /* ignore */ }

%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
