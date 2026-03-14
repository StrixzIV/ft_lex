%%
^start { print(f'START AT LINE: {yytext}') }
end$   { print(f'END AT LINE: {yytext}') }
.      { }
%%
