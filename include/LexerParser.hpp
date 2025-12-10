/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 01:41:56 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 01:41:56 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXERPARSER_HPP
#define LEXERPARSER_HPP

# include <string>
# include <string_view>
# include <vector>
# include <stdexcept>

class LexerParser {

    public:

        struct Rule {
            std::string regex;
            std::string action;
        };

        LexerParser(const std::string &filename);
        ~LexerParser() = default;

        void parse();

        const std::string &getDefinitions() const;
        const std::string &getRules() const;
        const std::string &getUserCode() const;
        const std::vector<Rule> &getRulesList() const;
    
    private:
        std::string _filename;
        std::string _content;
        std::string _definitions;
        std::string _rules;
        std::string _userCode;
        std::vector<Rule> _rulesList;

        void _readFile();
        void _splitSections();
        void _parseRules();

};

#endif
