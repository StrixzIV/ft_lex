#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <set>
#include <vector>

enum TokenType { CHAR, OPERATOR, CHARSET };

struct Token {
    TokenType type;
    char c;
    std::set<char> charSet;
    
    Token(char c, TokenType type) : type(type), c(c) {}
    Token(std::set<char> set) : type(CHARSET), c(0), charSet(set) {}
};

#endif
