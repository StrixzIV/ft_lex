%%
^test        { printf("START[%s]\n", yytext); }
end$         { printf("END[%s]\n", yytext); }
^both$       { printf("BOTH[%s]\n", yytext); }
[a-zA-Z]+    { printf("WORD[%s]\n", yytext); }
\n           { printf("NEWLINE\n"); }
.            { /* ignore */ }
%%
