/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:59 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:12:59 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEXPARSER_HPP
#define REGEXPARSER_HPP

#include "Token.hpp"
#include <string>
#include <vector>

class RegexParser {

  public:
    static std::vector<Token> toPostfix(const std::string &regex);

    static const char CONCAT_OP = 1;

  private:
    static int _getPrecedence(const Token &t);
    static std::vector<Token> _tokenize(const std::string &regex);
    static std::vector<Token>
    _addExplicitConcat(const std::vector<Token> &tokens);
};

#endif
