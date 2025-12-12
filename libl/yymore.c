/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yymore.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:59:06 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 02:59:07 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

extern int yyleng;
extern int yy_more_len;
extern int yy_more_flag;

int yymore(void) {
    yy_more_len = yyleng;
    yy_more_flag = 1;
    return 0;
}
