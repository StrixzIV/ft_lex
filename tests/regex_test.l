%{
#include <stdio.h>
%}

%%
(a|b)*abb   { action1 }
ab+c?         { action2 }
a(b|c)*d      { action3 }
%%

int main() { yylex(); }
