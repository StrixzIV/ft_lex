/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGenerator.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:13:32 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 03:19:34 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGENERATOR_HPP
#define CGENERATOR_HPP

# include "AGenerator.hpp"

# include <string>

class CGenerator: public AGenerator {

    public:
        CGenerator() = default;

    protected:
        std::string generateHeader(const LexerParser &parser) override;
        std::string generateTables(const DFA &dfa, const LexerParser &parser) override;
        std::string generateLexerBody(const LexerParser &parser) override;
        std::string generateUserCode(const LexerParser &parser) override;

    private:
        std::string _generateRulesSwitch(const LexerParser &parser); 

};

#endif
