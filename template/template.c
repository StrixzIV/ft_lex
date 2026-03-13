// Template for lex.yy.c generation. Placeholders start and end with __

__HEADER_PLACEHOLDER__

__TABLES_PLACEHOLDER__

/* --- Internal state for yylex --- */

FILE *yyin   = NULL;
FILE *yyout  = NULL;

char *yytext   = NULL;
int   yyleng   = 0;
int   yylineno = 1;

/* Current start condition (defaults to 0 / INITIAL) */
int   yy_start = 0;
int yy_at_bol = 1; // 1 if we are at the beginning of a line
#define BEGIN(state) (yy_start = (state))

/* Dynamic input buffer */
static char  *yy_buffer    = NULL;
static int    yy_buf_len   = 0;
static int    yy_buf_pos   = 0;
static int    yy_buf_cap   = 0;

/* Hold-char mechanism for input()/unput()/yyless() */
static unsigned char yy_hold_char          = 0;
static int           yy_hold_char_restored = 1;

/* yymore() support */
static int yy_more_flag = 0;
static int yy_more_len  = 0;

/* Forward declarations for libl linkage */
extern int yywrap(void);

/* --- Internal helpers --- */

static void yy_buf_ensure(int needed) {
    if (yy_buf_cap >= needed)
        return;
    int new_cap = (yy_buf_cap == 0) ? 256 : yy_buf_cap;
    while (new_cap < needed)
        new_cap *= 2;
    char *tmp = (char *)realloc(yy_buffer, new_cap);
    if (!tmp) {
        fprintf(stderr, "ft_lex: out of memory\n");
        exit(1);
    }
    yy_buffer = tmp;
    yy_buf_cap = new_cap;
}

/* Called by libl/unput.c */
void buffer_realloc(int needed) {
    yy_buf_ensure(needed + 1);
}

/* Called by libl/input.c */
int yy_read_char(void) {
    if (!yyin)
        yyin = stdin;
    return fgetc(yyin);
}

/* yylex Driver */
int yylex(void) {

    int current_state;
    int next_state;
    int last_accepting_state;
    int c;

    if (!yyin)
        yyin = stdin;
    if (!yyout)
        yyout = stdout;

    while (1) {

        current_state = yy_start_state_idx[yy_start];
        last_accepting_state = -1;

        int buf_idx = 0;
        int last_accepting_idx = -1;

        /* Handle yymore(): keep previous match text */
        if (yy_more_flag) {
            buf_idx = yy_more_len;
            yy_more_flag = 0;
        }

        yy_buf_ensure(buf_idx + 256);

        /* Read first char */
        c = fgetc(yyin);
        if (c == EOF) {
            int ret = yywrap();
            if (ret != 0) {
                return 0; // EOF reached and yywrap says stop
            }
            continue; // Retry reading if yywrap swapped the file
        }

        /* Check for Start of Line Anchor (256) */
        if (yy_at_bol) {
            int anchor_state = yy_nxt[current_state][256];
            if (anchor_state != -1) {
                current_state = anchor_state;
                if (yy_accept[current_state] != -1) {
                    last_accepting_state = current_state;
                    last_accepting_idx = buf_idx;
                }
            }
        }

        /* Push char to buffer */
        yy_buffer[buf_idx++] = (char)c;
        yy_buffer[buf_idx] = '\0';

        /* Check for End of Line Anchor (257) on '\n' */
        if (c == '\n') {
            int eol_state = yy_nxt[current_state][257];
            if (eol_state != -1) {
                if (yy_accept[eol_state] != -1) {
                    // Match found before the '\n'
                    last_accepting_state = eol_state;
                    last_accepting_idx = buf_idx - 1;
                }
            }
        }

        /* Initial transition */
        next_state = yy_nxt[current_state][(unsigned char)c];

        while (next_state != -1) {

            current_state = next_state;

            if (yy_accept[current_state] != -1) {
                last_accepting_state = current_state;
                last_accepting_idx = buf_idx;
            }

            c = fgetc(yyin);
            if (c == EOF) {
                /* Check for End of Line Anchor (257) on EOF */
                int eol_state = yy_nxt[current_state][257];
                if (eol_state != -1) {
                    if (yy_accept[eol_state] != -1) {
                        last_accepting_state = eol_state;
                        last_accepting_idx = buf_idx;
                    }
                }
                break;
            }

            yy_buf_ensure(buf_idx + 2);
            yy_buffer[buf_idx++] = (char)c;
            yy_buffer[buf_idx] = '\0';

            /* Check for End of Line Anchor (257) on '\n' */
            if (c == '\n') {
                int eol_state = yy_nxt[current_state][257];
                if (eol_state != -1) {
                    if (yy_accept[eol_state] != -1) {
                        last_accepting_state = eol_state;
                        last_accepting_idx = buf_idx - 1;
                    }
                }
            }

            next_state = yy_nxt[current_state][(unsigned char)c];
        }

        /* No more transitions. */
        if (last_accepting_state != -1) {
            /* Match found — push back characters past the match */
            while (buf_idx > last_accepting_idx) {
                int ch = (unsigned char)yy_buffer[--buf_idx];
                ungetc(ch, yyin);
            }
            yy_buffer[buf_idx] = '\0';
            
            // Re-calc yylineno based on actual matched chars
            for (int i = 0; i < buf_idx; i++) {
                if (yy_buffer[i] == '\n')
                    yylineno++;
            }

            yytext = yy_buffer;
            yyleng = buf_idx;

            // Update yy_at_bol
            if (yyleng > 0 && yytext[yyleng - 1] == '\n') {
                yy_at_bol = 1;
            } else if (yyleng > 0) {
                yy_at_bol = 0;
            }

            /* Save the hold char for input()/unput()/yyless() */
            yy_hold_char = yy_buffer[yyleng];
            yy_hold_char_restored = 0;

            /* Set buffer position for libl functions */
            yy_buf_pos = buf_idx;
            yy_buf_len = buf_idx;

            switch (yy_accept[last_accepting_state]) {
__YYLEX_BODY_PLACEHOLDER__
            }

            /* Restore hold char after action */
            if (!yy_hold_char_restored) {
                yy_buffer[yyleng] = yy_hold_char;
                yy_hold_char_restored = 1;
            }
        } else {
            /* No match — ECHO first char, push back the rest */
            if (buf_idx == 0) {
                 // Should never happen, but safety override
                 break;
            }
            fputc((unsigned char)yy_buffer[0], yyout);
            while (buf_idx > 1) {
                char ch = yy_buffer[--buf_idx];
                ungetc((unsigned char)ch, yyin);
            }
            if (yy_buffer[0] == '\n') {
                yy_at_bol = 1;
                yylineno++;
            } else {
                yy_at_bol = 0;
            }
        }
    }
}

__USER_CODE_PLACEHOLDER__