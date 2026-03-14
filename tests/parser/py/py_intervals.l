%%
[0-9]{3}   { print(f'TRIPLE: {yytext}') }
[a-z]{1,2} { print(f'SHORT: {yytext}') }
.          { }
%%
