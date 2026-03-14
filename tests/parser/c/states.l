%x COMMENT STRING

%%

\/\*            { BEGIN(COMMENT); }
<COMMENT>\*\/   { BEGIN(INITIAL); }
<COMMENT>.|\n   { /* ignore comment content */ }

\"              { BEGIN(STRING); printf("String: "); }
<STRING>\"      { BEGIN(INITIAL); printf("\n"); }
<STRING>.       { printf("%s", yytext); }

[a-zA-Z]+       { printf("WORD[%s]\n", yytext); }
[ \t\n]+        { /* ignore whitespace */ }
.               { printf("Other: %c\n", yytext[0]); }

%%
