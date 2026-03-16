/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:58:47 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/14 11:05:36 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libl.h"

int input(void) {
  yy_restore_hold_char();
  int c = yy_read_char();
  if (c == '\n') {
    yylineno++;
    yy_at_bol = 1;
  } else if (c != EOF) {
    yy_at_bol = 0;
  }
  return c;
}
