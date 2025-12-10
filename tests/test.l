%{
#include <stdio.h>
%}

%%
"test"  { printf("Found test\n"); }
.       { printf("Found char: %s\n", yytext); }
%%
