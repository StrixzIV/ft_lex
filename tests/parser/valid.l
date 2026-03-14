%{
#include <stdio.h>
%}

%%
test  { printf("Found test\n"); }
%%

int main() { yylex(); }
