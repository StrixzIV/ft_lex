/* Debug test - character classes */
%%
[a-z]+      { printf("[LOWER:%s]", yytext); }
[A-Z]+      { printf("[UPPER:%s]", yytext); }
[0-9]+      { printf("[DIGIT:%s]", yytext); }
[ \t\n]+    { /* skip whitespace */ }
.           { printf("[OTHER:%s]", yytext); }
%%
