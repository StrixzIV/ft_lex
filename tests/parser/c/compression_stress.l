%%
[ \t\n]+                 /* ignore whitespace */
[a-zA-Z0-9_][a-zA-Z0-9_]*  printf("WORD: %s\n", yytext);
[0-9]+(\.[0-9]+)?        printf("NUMBER: %s\n", yytext);
\"([^\"\\]|\\.)*\"       printf("STRING: %s\n", yytext);
"if"|"else"|"while"|"for"  printf("KEYWORD: %s\n", yytext);
[+*/%=-]                 printf("OPERATOR: %s\n", yytext);
.                        printf("CHAR: %s\n", yytext);
%%
