/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:58:47 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 02:58:48 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdint.h>

extern char *yytext;
extern int yyleng;
extern uint8_t yy_hold_char;
extern int yy_hold_char_restored;
extern int yy_read_char(void);

int input(void) {
    if (!yy_hold_char_restored) {
        yytext[yyleng] = yy_hold_char;
        yy_hold_char_restored = 1;
    }
    int c = yy_read_char();
    return c == EOF ? 0 : c;
}
