#include <stdio.h>

extern int yylex(void);

int main(void) {
    return yylex();
}
