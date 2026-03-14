%{
count = 0
%}
%%
[a-zA-Z]+ { global count; count += 1 }
.         { }
\n        { }
<<EOF>>   { print(f'Total words: {count}'); return 0 }
%%
