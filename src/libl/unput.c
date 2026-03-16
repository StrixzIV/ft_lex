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

  yy_restore_hold_char();

  if (c == '\n') {
    yylineno--;
    /* yy_at_bol restoration after unput('\n') is best-effort.
       We can't easily know if the char BEFORE this new BOL is '\n'
       without peeking into the stream, which is not always possible. */
    yy_at_bol = 0; // Defaulting to 0 after unput('\n') as we are now BEFORE the newline
  } else {
    yy_at_bol = 0;
  }

  ungetc((unsigned char)c, yyin);
  return c;
}
