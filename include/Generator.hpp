/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Generator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 03:01:32 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 03:01:32 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "DFA.hpp"
#include "LexerParser.hpp"
#include <iostream>

class Generator {
public:
    static void generate(const DFA& dfa, const LexerParser& parser, std::ostream& out);

private:
    static void _writeHeader(const LexerParser& parser, std::ostream& out);
    static void _writeTables(const DFA& dfa, std::ostream& out);
    static void _writeYYLex(const DFA& dfa, const LexerParser& parser, std::ostream& out);
    static void _writeUserCode(const LexerParser& parser, std::ostream& out);
};

#endif
