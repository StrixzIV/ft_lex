/* Multi-line action test */
%%
[a-z]+    {
    printf("word: %s\n", yytext);
}
[0-9]+    { printf("num: %s\n", yytext); }
.|\n      { /* skip */ }
%%
