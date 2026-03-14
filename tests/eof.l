%{
#include <stdio.h>
int eof_reached = 0;
%}

%x BYE

%%

"hello"   { printf("Hello found\n"); }
"quit"    { printf("Entering BYE state\n"); BEGIN(BYE); }

<BYE><<EOF>> {
    printf("Goodbye from BYE state!\n");
    eof_reached = 1;
    return 0;
}

<<EOF>> {
    printf("Reached end of file normally\n");
    eof_reached = 1;
    return 0;
}

%%

int main() {
    yylex();
    if (eof_reached) {
        printf("Success: EOF handler triggered\n");
    } else {
        printf("Failure: EOF handler NOT triggered\n");
    }
    return 0;
}

int yywrap() {
    return 1;
}
