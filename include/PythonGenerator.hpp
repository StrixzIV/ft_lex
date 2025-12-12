/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PythonGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:14:38 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 01:29:45 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PYTHONGNERATOR_HPP
#define PYTHONGNERATOR_HPP

# include "AGenerator.hpp"

# include <string>

class PythonGenerator: public AGenerator {

    public:
        PythonGenerator() = default;
    
    protected:
        std::string generateHeader(const LexerParser &parser) override;
        std::string generateTables(const DFA &dfa) override;
        std::string generateLexerBody(const LexerParser &parser) override;
        std::string generateUserCode(const LexerParser &parser) override;

};

#endif