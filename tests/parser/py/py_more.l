%%
a { print('FOUND A'); yymore() }
b { print(f'FOUND B WITH TEXT: {yytext}') }
. { }
%%
