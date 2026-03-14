DIGIT    [0-9]
LETTER   [a-zA-Z]
%%
{DIGIT}+     { printf("[NUM:%s]", yytext); }
{LETTER}+    { printf("[WORD:%s]", yytext); }
.|\n         { /* skip */ }
%%
