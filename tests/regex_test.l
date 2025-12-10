%{
#include <stdio.h>
%}

%%
(a|b)*abb   { printf("Action 1\n"); }
ab+c?         { printf("Action 2\n"); }
a(b|c)*d      { printf("Action 3\n"); }
%%

int main() { yylex(); }
