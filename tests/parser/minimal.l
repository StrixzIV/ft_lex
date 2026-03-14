/* Minimal test - no character classes */
%%
hello    { printf("HELLO"); }
world    { printf("WORLD"); }
[0-9]    { printf("DIGIT"); }
.        { printf("OTHER"); }
%%
