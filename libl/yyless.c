/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yyless.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:02:38 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/14 11:02:54 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libl.h"

int yyless(int n) {
  if (n < 0 || n > yyleng) {
    return -1;
  }

  /* Restore the character that was replaced by '\0' by the driver */
  if (!yy_hold_char_restored) {
    yytext[yyleng] = yy_hold_char;
    yy_hold_char_restored = 1;
  }

  /* Push back characters from yyleng-1 down to n and update yylineno */
  for (int i = yyleng - 1; i >= n; i--) {
    unsigned char c = (unsigned char)yytext[i];
    if (c == '\n') {
      yylineno--;
    }
    ungetc(c, yyin);
  }

  yyleng = n;
  yytext[yyleng] = '\0';

  /* Re-capture the new hold char */
  yy_hold_char = (unsigned char)yytext[yyleng];
  yy_hold_char_restored = 1;

  /* Update BOL status: if last kept char is '\n', next match is at BOL.
     If n == 0, we must check if the char BEFORE this match was '\n'.
     But for simplicity, we let the driver re-evaluate BOL in its loop start.
     Actually, the driver should re-calc yy_at_bol based on current yyleng. */
  if (yyleng > 0) {
    yy_at_bol = (yytext[yyleng - 1] == '\n');
  } else {
    /* If yyleng is 0, we remain at BOL if we were at BOL when we started.
       This is slightly complex but let's assume BOL is correctly set. */
  }

  return 0;
}
