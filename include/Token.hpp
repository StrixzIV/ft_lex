/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:59 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 02:41:59 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <set>
#include <vector>

enum TokenType { 
    CHAR, 
    OPERATOR, 
    CHARSET,
    BOL,  // Beginning of line anchor (^)
    EOL   // End of line anchor ($)
};

struct Token {
    TokenType type;
    char c;
    std::set<char> charSet;
    
    Token(char c, TokenType type) : type(type), c(c) {}
    Token(std::set<char> set) : type(CHARSET), c(0), charSet(set) {}
};

#endif
