/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:59 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:12:59 by jikaewsi         ###   ########.fr       */
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
    ANCHOR_START,          // ^ start of line (pseudo-char 256)
    ANCHOR_END,            // $ end of line   (pseudo-char 257)
    TRAILING_CONTEXT_OP,   // / trailing context operator
    INTERVAL               // {n,m}
};

struct Token {
    TokenType type;
    int c; // Changed to int to allow values > 255 for anchors
    std::set<int> charSet; 
    int min; // For INTERVAL
    int max; // For INTERVAL (-1 for infinity)
    
    Token(int c, TokenType type) : type(type), c(c), min(0), max(0) {}
    Token(std::set<int> set) : type(CHARSET), c(0), charSet(set), min(0), max(0) {}
    Token(int min, int max) : type(INTERVAL), c(0), min(min), max(max) {}
};

#endif
