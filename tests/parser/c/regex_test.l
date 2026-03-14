%%
[[:digit:]]+ { printf("DIGITS[%s]", yytext); }
[[:alpha:]]{3}  { printf("ALPHA3[%s]", yytext); }
[A-Z]{1,2}      { printf("UPPER12[%s]", yytext); }
.               { printf("%s", yytext); }
%%
