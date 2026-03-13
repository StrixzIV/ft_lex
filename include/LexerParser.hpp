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

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <stdexcept>

class LexerParser {

public:
    struct Rule {
        std::string regex;
        std::string action;
        std::vector<std::string> startConditions; // Which conditions this rule applies to
    };

    struct StartCondition {
        std::string name;
        bool isExclusive;
    };

    LexerParser(const std::string &filename);
    ~LexerParser() = default;

    void parse();

    const std::string &getDefinitions() const;
    const std::string &getRules() const;
    const std::string &getUserCode() const;
    const std::vector<Rule> &getRulesList() const;
    const std::map<std::string, std::string> &getNamedDefinitions() const;
    const std::vector<StartCondition> &getStartConditions() const;
    
private:
    std::string _filename;
    std::string _content;
    std::string _definitions;
    std::string _rules;
    std::string _userCode;
    std::string _rawDefinitions;
    std::vector<Rule> _rulesList;
    std::map<std::string, std::string> _namedDefinitions;
    std::vector<StartCondition> _startConditions;

    void _readFile();
    void _splitSections();
    void _parseDefinitions();
    std::string _expandDefinitions(const std::string &regex);
    void _parseRules();

};

#endif
