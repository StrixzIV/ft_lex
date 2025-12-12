/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Generator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 03:01:32 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 00:46:20 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "DFA.hpp"
#include "LexerParser.hpp"
#include <iostream>

class Generator {

    public:
        static void generate(const DFA &dfa, const LexerParser &parser, std::ostream &out);

    private:
        static std::string _generateHeader(const LexerParser &parser);
        static std::string _generateTables(const DFA &dfa);
        static std::string _generateYYLexRules(const LexerParser &parser); 
        static std::string _generateUserCode(const LexerParser &parser);

};

#endif
