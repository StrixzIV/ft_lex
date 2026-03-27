/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libl.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:53:00 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/14 11:56:16 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBL_H
# define LIBL_H

# include <stdio.h>
# include <string.h>

/* --- Shared variables --- */

extern FILE *yyin;
extern FILE *yyout;

// yytext is either char * or char[] depending on %pointer / %array
// We should NOT declare it here as its type varies.
// Functions that need to interact with yytext should call helpers in lex.yy.c.
extern int yyleng;
extern int yylineno;

extern int yy_start;
extern int yy_at_bol;

extern char *yy_buffer;
extern int yy_buf_len;
extern int yy_buf_pos;
extern int yy_buf_cap;

extern unsigned char yy_hold_char;
extern int yy_hold_char_restored;

extern int yy_more_flag;
extern int yy_more_len;

/* --- Macros --- */

#define BEGIN (yy_start) =
#define INITIAL 0
#define YY_START (yy_start)

/* Standard Flex-like macros for user actions */
#ifndef ECHO
# define ECHO (void)fwrite(yytext, yyleng, 1, yyout)
#endif

#ifndef YY_STATE_EOF
# define YY_STATE_EOF(state) (YY_START == (state))
#endif

/* --- Library functions --- */

int yylex(void);
int yylex_destroy(void);
int yywrap(void);
int input(void);
int unput(int c);
int yymore(void);
int yyless(int n);

/* --- Internal helpers --- */

void buffer_realloc(int needed);
int yy_read_char(void);
void yy_restore_hold_char(void);
int yy_buf_ensure(int needed);

#endif
