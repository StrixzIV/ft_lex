/* Simpler test lexer - no negated character classes */
%{
#include <stdio.h>
%}

%s COMMENT

%%

"/*"            { printf("[COMMENT_START]"); BEGIN(COMMENT); }
<COMMENT>"*/"   { printf("[COMMENT_END]"); BEGIN(INITIAL); }
<COMMENT>.      { /* skip */ }

[0-9]+          { printf("[NUM:%s]", yytext); }
[a-zA-Z]+       { printf("[WORD:%s]", yytext); }
[ \t]+          { /* skip whitespace */ }
\n              { printf("\n"); }
.               { printf("[CHAR:%s]", yytext); }

%%

int main(void) {
    printf("=== Test Start ===\n");
    yylex();
    printf("\n=== Test End ===\n");
    return 0;
}
