/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:19:29 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 01:19:44 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string replace_placeholder(std::string &source, const std::string_view &placeholder, const std::string &replacement) {
    
    std::string::size_type pos = source.find(placeholder.data());
    
    if (pos != std::string::npos) {
        source.replace(pos, placeholder.length(), replacement);
    }
    
    return source;

}
