/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 01:42:12 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 01:42:12 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LexerParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

LexerParser::LexerParser(const std::string &filename) : _filename(filename) {}

const std::string &LexerParser::getDefinitions() const {
    return _definitions;
}

const std::string &LexerParser::getRules() const {
    return _rules;
}

const std::string &LexerParser::getUserCode() const {
    return _userCode;
}

void LexerParser::_readFile() {

    std::ifstream file(_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + _filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    _content = buffer.str();

}

void LexerParser::_splitSections() {

    size_t first_sep_pos = _content.find("%%");

    if (first_sep_pos == std::string::npos) {
        throw std::runtime_error("Invalid format: Missing first '%%' delimiter");
    }

    _definitions = _content.substr(0, first_sep_pos);
    
    size_t pos;
    
    while ((pos = _definitions.find("%{")) != std::string::npos) {
        _definitions.erase(pos, 2);
    }

    while ((pos = _definitions.find("%}")) != std::string::npos) {
        _definitions.erase(pos, 2);
    }

    size_t start_rules_pos = first_sep_pos + 2;
    size_t second_sep_pos = _content.find("%%", start_rules_pos);

    if (second_sep_pos == std::string::npos) {
        _rules = _content.substr(start_rules_pos);
        _userCode = "";
    } else {
        _rules = _content.substr(start_rules_pos, second_sep_pos - start_rules_pos);
        _userCode = _content.substr(second_sep_pos + 2);
    }

}

void LexerParser::parse() {
    _readFile();
    _splitSections();
    _parseRules();
}

const std::vector<LexerParser::Rule> &LexerParser::getRulesList() const {
    return _rulesList;
}

void LexerParser::_parseRules() {
    std::istringstream stream(_rules);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Simple trim
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue; // Empty line
        }
        
        std::string trimmed = line.substr(first);
        if (trimmed.empty()) {
            continue;
        }

        // Find split between regex and action
        // Regex is the first word (whitespace delimited), Action is the rest
        size_t split = trimmed.find_first_of(" \t");
        
        Rule rule;
        if (split == std::string::npos) {
            rule.regex = trimmed;
            rule.action = ""; // No action specified?
        } else {
            rule.regex = trimmed.substr(0, split);
            size_t actionStart = trimmed.find_first_not_of(" \t", split);
            if (actionStart != std::string::npos) {
                rule.action = trimmed.substr(actionStart);
            }
        }
        _rulesList.push_back(rule);
    }
}
