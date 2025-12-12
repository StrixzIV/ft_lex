/* Test lexer for POSIX compliance verification */
%{
#include <stdio.h>
#include <string.h>
%}

%s COMMENT
%x STRING

%%

"/*"            { printf("[COMMENT_START]"); BEGIN(COMMENT); }
<COMMENT>"*/"   { printf("[COMMENT_END]"); BEGIN(INITIAL); }
<COMMENT>.      { /* skip comment content */ }
<COMMENT>\n     { /* skip newlines in comment */ }

\"              { printf("[STRING_START]"); BEGIN(STRING); }
<STRING>\"      { printf("[STRING_END]"); BEGIN(INITIAL); }
<STRING>[^"]+   { printf("[STRING:%s]", yytext); }

[0-9]+          { printf("[NUM:%s]", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*  { printf("[ID:%s]", yytext); }
[+\-*/=]        { printf("[OP:%s]", yytext); }
[ \t]+          { /* skip whitespace */ }
\n              { printf("\n"); }
.               { printf("[CHAR:%s]", yytext); }

%%

int main(void) {
    printf("=== POSIX Compliance Test ===\n");
    yylex();
    printf("\n=== Test Complete ===\n");
    return 0;
}
