/* Test anchors: ^ (BOL) and $ (EOL) */
%%
^hello      { printf("[BOL_HELLO]"); }
world$      { printf("[WORLD_EOL]"); }
^test$      { printf("[BOTH_TEST]"); }
[a-z]+      { printf("[WORD:%s]", yytext); }
[ \t]+      { /* skip whitespace */ }
\n          { printf("[NL]"); }
.           { printf("[OTHER:%s]", yytext); }
%%
