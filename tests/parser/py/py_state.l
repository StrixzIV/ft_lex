%x COMMENT
%%
"/*"         { self.BEGIN(self.COMMENT); print('Entering comment') }
<COMMENT>"*/" { self.BEGIN(self.INITIAL); print('Exiting comment') }
<COMMENT>.    { }
[a-z]+        { print(f'WORD: {yytext}') }
.             { }
%%
