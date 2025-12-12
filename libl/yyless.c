/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yyless.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:59:02 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 02:59:03 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stddef.h>

extern char *yytext;
extern int yyleng;
extern uint8_t yy_hold_char;
extern size_t yy_buf_pos;

int yyless(int n) {
    yytext[yyleng] = yy_hold_char;
    yy_hold_char = yytext[n];
    yy_buf_pos = yy_buf_pos - yyleng + n;
    yyleng = n;
    yytext[yyleng] = '\0';
    return 0;
}
