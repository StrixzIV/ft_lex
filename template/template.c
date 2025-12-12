// Template for lex.yy.c generation. Placeholders start and end with __

__HEADER_PLACEHOLDER__

__TABLES_PLACEHOLDER__

/* --- Scanner State Variables --- */
char *yytext = NULL;
int yyleng = 0;
uint8_t yy_hold_char = 0;
int yy_hold_char_restored = 1;

/* Buffer management */
char *yy_buffer = NULL;
size_t yy_buf_pos = 0;
int yy_buf_len = 0;
size_t yy_buf_size = 0;

/* yymore support */
int yy_more_flag = 0;
int yy_more_len = 0;

/* --- Start Conditions --- */
int yy_start = 0;
#define BEGIN(x) (yy_start = (x))
#define INITIAL 0

/* Scanner functions provided by libl (link with -ll) */
extern int input(void);
extern int unput(int c);
extern int yyless(int n);
extern int yymore(void);
extern int yywrap(void);

/* Buffer reallocation helper */
void buffer_realloc(size_t new_size) {
    if (new_size > yy_buf_size) {
        size_t alloc_size = new_size + 256;
        yy_buffer = (char *)realloc(yy_buffer, alloc_size);
        yy_buf_size = alloc_size;
    }
}

/* Read a character from input */
int yy_read_char(void) {
    if (yy_buf_pos < (size_t)yy_buf_len) {
        return (unsigned char)yy_buffer[yy_buf_pos++];
    }
    int c = getchar();
    if (c == EOF) {
        if (yywrap()) {
            return EOF;
        }
        return yy_read_char();
    }
    buffer_realloc(yy_buf_len + 1);
    yy_buffer[yy_buf_len++] = (char)c;
    yy_buf_pos = yy_buf_len;
    return c;
}

/* yylex Driver */
int yylex(void) {
    
    int current_state;
    int next_state;
    int last_accepting_state;
    int c;
    
    static char text_buffer[4096];
    yytext = text_buffer;
    
    while (1) {
    
        current_state = 0;
        last_accepting_state = -1;
        
        int text_idx = yy_more_len;
        yy_more_len = 0;
        int last_accepting_idx = -1;
        
        yy_hold_char_restored = 1;
        
        // Read first char
        c = yy_read_char();
        if (c == EOF) return 0;
        
        text_buffer[text_idx++] = (char)c;
        text_buffer[text_idx] = '\0';
        
        // Initial transition
        next_state = yy_nxt[current_state][(unsigned char)c];
        
        while (next_state != -1) {

            current_state = next_state;
            
            if (yy_accept[current_state] != -1) {
                last_accepting_state = current_state;
                last_accepting_idx = text_idx;
            }
            
            c = yy_read_char();
            if (c == EOF) break;
            
            if (text_idx >= 4095) {
                fprintf(stderr, "ft_lex: buffer overflow\n");
                exit(1);
            }
            
            text_buffer[text_idx++] = (char)c;
            text_buffer[text_idx] = '\0';
            next_state = yy_nxt[current_state][(unsigned char)c];
        
        }
        
        // No more transitions.
        if (last_accepting_state != -1) {
            int rule_id = yy_accept[last_accepting_state];
            
            // Check anchor constraints
            int anchor_ok = 1;
            if (rule_id >= 0) {
                // Check BOL anchor - must be at beginning of line
                if (yy_rule_bol[rule_id] && !yy_at_bol) {
                    anchor_ok = 0;
                }
                // Check EOL anchor - next char must be newline or EOF
                if (yy_rule_eol[rule_id]) {
                    int peek = yy_read_char();
                    if (peek != '\n' && peek != EOF) {
                        anchor_ok = 0;
                    }
                    if (peek != EOF) {
                        unput(peek);
                    }
                }
            }
            
            if (!anchor_ok) {
                // Anchor check failed - treat as no match
                putchar(text_buffer[0]);
                for (int i = text_idx - 1; i > 0; i--) {
                    unput((unsigned char)text_buffer[i]);
                }
                yy_at_bol = (text_buffer[0] == '\n');
                continue;
            }
            
            // Match found! Backtrack extra chars.
            while (text_idx > last_accepting_idx) {
                unput((unsigned char)text_buffer[--text_idx]);
            }
            
            // Save hold char and null-terminate
            yy_hold_char = text_buffer[text_idx];
            text_buffer[text_idx] = '\0';
            yy_hold_char_restored = 0;
            
            yytext = text_buffer;
            yyleng = text_idx;
            
            switch (yy_accept[last_accepting_state]) {
__YYLEX_BODY_PLACEHOLDER__
            }
            
            // Update yy_at_bol based on whether yytext ended with newline
            yy_at_bol = (yyleng > 0 && yytext[yyleng - 1] == '\n');
            
            // Restore hold char after action
            if (!yy_hold_char_restored) {
                text_buffer[yyleng] = yy_hold_char;
                yy_hold_char_restored = 1;
            }
            
            // Handle yymore()
            if (yy_more_flag) {
                yy_more_len = yyleng;
                yy_more_flag = 0;
            }
        } else {
            // No match. Echo first char and retry.
            putchar(text_buffer[0]);
            yy_at_bol = (text_buffer[0] == '\n');
            for (int i = text_idx - 1; i > 0; i--) {
                unput((unsigned char)text_buffer[i]);
            }
        }
    }
}

__USER_CODE_PLACEHOLDER__