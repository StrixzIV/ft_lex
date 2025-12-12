// Template for lex.yy.c generation. Placeholders start and end with __

__HEADER_PLACEHOLDER__

__TABLES_PLACEHOLDER__

/* yylex Driver */
int yylex(void) {
    
    int current_state;
    int next_state;
    int last_accepting_state = -1;
    char c;
    
    while (1) {
    
        current_state = 0; // Start state assumption (Index 0)
        last_accepting_state = -1;
        
        static char buffer[4096];
        int buf_idx = 0;
        int last_accepting_idx = -1;
        
        // Read first char
        c = getchar();
        if (c == EOF) return 0;
        buffer[buf_idx++] = c;
        buffer[buf_idx] = 0;
        
        // Initial transition
        next_state = yy_nxt[current_state][(unsigned char)c];
        
        while (next_state != -1) {

            current_state = next_state;
            
            if (yy_accept[current_state] != -1) {
                last_accepting_state = current_state;
                last_accepting_idx = buf_idx;
            }
            
            c = getchar();
            if (c == EOF) break;
            if (buf_idx >= 4095) { fprintf(stderr, "Buffer overflow\n"); exit(1); }
            
            buffer[buf_idx++] = c;
            buffer[buf_idx] = 0;
            next_state = yy_nxt[current_state][(unsigned char)c];
        
        }
        
        // No more transitions.
        if (last_accepting_state != -1) {
            // Match found! Backtrack.
            while (buf_idx > last_accepting_idx) {
                ungetc(buffer[--buf_idx], stdin);
            }
            buffer[buf_idx] = 0; // Terminate yytext at match end
            
            yytext = buffer;
            yyleng = buf_idx;
            
            switch (yy_accept[last_accepting_state]) {
__YYLEX_RULES_PLACEHOLDER__
            }
        } else {
            // No match. Echo char and unput everything else.
            printf("%c", buffer[0]);
            while (buf_idx > 1) {
                ungetc(buffer[--buf_idx], stdin);
            }
        }
    }
}

__USER_CODE_PLACEHOLDER__