/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lex.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 00:00:00 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/14 00:00:00 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LEX_H
# define FT_LEX_H

# include <stdio.h>
# include <stdint.h>
# include <stddef.h>

/* --- Shared variables --- */

extern FILE *yyin;
extern FILE *yyout;

extern char *yytext;
extern int yyleng;
extern int yylineno;

extern int yy_start;
extern int yy_at_bol;

extern char *yy_buffer;
extern int yy_buf_len;
extern int yy_buf_pos;
extern int yy_buf_cap;

extern uint8_t yy_hold_char;
extern int yy_hold_char_restored;

extern int yy_more_flag;
extern int yy_more_len;

/* --- Library functions --- */

int yylex(void);
int yywrap(void);
int yyless(int n);
int input(void);
int unput(int c);
int yymore(void);

/* --- Internal helpers --- */

void buffer_realloc(int needed);
int yy_read_char(void);

#endif
