%{
#include <stdio.h>

/* Define variables to hold our counts */
int lines = 0;
int words = 0;
int numbers = 0;
%}

%%

[a-zA-Z]+   { printf("word\n"); words++; }        /* Rule 1: If it's a word, increment word count */
[0-9]+      { printf("number\n"); numbers++; }      /* Rule 2: If it's a number, increment number count */
\n          { printf("line\n"); lines++; }        /* Rule 3: If it's a newline, increment line count */
.           { }                 /* Rule 4: Ignore everything else (spaces, punctuation) */

%%

/* SECTION 3: USER CODE */

/* yywrap is called when input is exhausted. Return 1 to stop. */
int yywrap() {
    return 1;
}

int main() {
    printf("Enter text (Press Ctrl+D to end input):\n");
    
    /* yylex() starts the scanning process */
    yylex(); 
    
    printf("\n--- Results ---\n");
    printf("Lines: %d\n", lines);
    printf("Words: %d\n", words);
    printf("Numbers: %d\n", numbers);
    
    return 0;
}