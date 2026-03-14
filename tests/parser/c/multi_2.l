{DIGIT}+ { printf("NUM[%s]", yytext); }
.        { printf("CHAR[%s]", yytext); }
%%
