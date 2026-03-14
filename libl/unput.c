/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unput.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:58:58 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/14 11:06:14 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libl.h"

int unput(int c) {
  if (c == EOF) {
    return EOF;
  }

  if (!yy_hold_char_restored) {
    yytext[yyleng] = yy_hold_char;
    yy_hold_char_restored = 1;
  }

  if (c == '\n') {
    yylineno--;
    /* We can't easily know if the char BEFORE this new BOL is '\n'
       to set yy_at_bol correctly, but the driver loop will handle it. */
  }

  ungetc((unsigned char)c, yyin);
  return c;
}
