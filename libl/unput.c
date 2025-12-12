/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unput.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:58:58 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 02:58:59 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <string.h>

extern int yy_hold_char_restored;
extern uint8_t yy_hold_char;
extern int yy_buf_len;
extern char *yy_buffer;
extern size_t yy_buf_pos;
extern void buffer_realloc(size_t);

int unput(int c) {
    char *yy_cp;

    buffer_realloc(yy_buf_len + 1);

    if (!yy_hold_char_restored) {
        yy_cp = &yy_buffer[yy_buf_pos];
        *yy_cp = yy_hold_char;
    }

    memmove(&yy_buffer[yy_buf_pos + 1], &yy_buffer[yy_buf_pos], (yy_buf_len - yy_buf_pos));
    yy_buffer[yy_buf_pos] = (unsigned char) c;
    yy_buf_len += 1;

    if (!yy_hold_char_restored) {
        yy_hold_char = *yy_cp;
    }

    return c;
}
