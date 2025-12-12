/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AGenerator.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:11:33 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 01:26:07 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AGENERATOR_HPP
#define AGENERATOR_HPP

# include "DFA.hpp"
# include "LexerParser.hpp"

# include <string>
# include <iostream>
# include <typeinfo>

class CGenerator;
class PythonGenerator;

class AGenerator {

    public:
        void generate(const DFA &dfa, const LexerParser &parser, std::ostream &out);
        virtual ~AGenerator() = default;

    protected:

        virtual std::string generateHeader(const LexerParser &parser) = 0;
        virtual std::string generateTables(const DFA &dfa) = 0;
        virtual std::string generateLexerBody(const LexerParser &parser) = 0;
        virtual std::string generateUserCode(const LexerParser &parser) = 0;
        
        std::string loadTemplate(const std::string &template_key);

};

#endif
