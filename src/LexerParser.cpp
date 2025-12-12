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
    _parseDefinitions();
    _parseRules();
}

const std::vector<LexerParser::Rule> &LexerParser::getRulesList() const {
    return _rulesList;
}

const std::vector<LexerParser::StartCondition> &LexerParser::getStartConditions() const {
    return _startConditions;
}

void LexerParser::_parseDefinitions() {
    std::istringstream stream(_definitions);
    std::string line;
    std::string filteredDefinitions;
    
    while (std::getline(stream, line)) {
        // Trim leading whitespace
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            filteredDefinitions += line + "\n";
            continue;
        }
        
        std::string trimmed = line.substr(first);
        
        // Check for %s (inclusive) or %x (exclusive) start conditions
        if (trimmed.rfind("%s ", 0) == 0 || trimmed.rfind("%s\t", 0) == 0) {
            // Parse inclusive start conditions
            std::istringstream names(trimmed.substr(2));
            std::string name;
            while (names >> name) {
                _startConditions.push_back({name, false});
            }
            // Don't add to filteredDefinitions - this line is consumed
        } else if (trimmed.rfind("%x ", 0) == 0 || trimmed.rfind("%x\t", 0) == 0) {
            // Parse exclusive start conditions  
            std::istringstream names(trimmed.substr(2));
            std::string name;
            while (names >> name) {
                _startConditions.push_back({name, true});
            }
            // Don't add to filteredDefinitions - this line is consumed
        } else {
            // Regular definition line - keep it
            filteredDefinitions += line + "\n";
        }
    }
    
    _definitions = filteredDefinitions;
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
        
        Rule rule;
        
        // Check for start condition prefix: <STATE1,STATE2>pattern
        if (trimmed[0] == '<') {
            size_t closeAngle = trimmed.find('>');
            if (closeAngle != std::string::npos) {
                std::string conditions = trimmed.substr(1, closeAngle - 1);
                trimmed = trimmed.substr(closeAngle + 1);
                
                // Parse comma-separated condition names
                std::istringstream condStream(conditions);
                std::string cond;
                while (std::getline(condStream, cond, ',')) {
                    // Trim whitespace
                    size_t s = cond.find_first_not_of(" \t");
                    size_t e = cond.find_last_not_of(" \t");
                    if (s != std::string::npos) {
                        rule.startConditions.push_back(cond.substr(s, e - s + 1));
                    }
                }
            }
        }

        // Find split between regex and action
        // Need to skip whitespace inside character classes [...] and quoted strings
        size_t split = std::string::npos;
        bool inBracket = false;
        bool inQuote = false;
        bool escaped = false;
        
        for (size_t j = 0; j < trimmed.size(); ++j) {
            char ch = trimmed[j];
            
            if (escaped) {
                escaped = false;
                continue;
            }
            
            if (ch == '\\') {
                escaped = true;
                continue;
            }
            
            if (ch == '[' && !inQuote) {
                inBracket = true;
                continue;
            }
            
            if (ch == ']' && inBracket && !inQuote) {
                inBracket = false;
                continue;
            }
            
            if (ch == '"' && !inBracket) {
                inQuote = !inQuote;
                continue;
            }
            
            // Only split on whitespace if not inside brackets or quotes
            if (!inBracket && !inQuote && (ch == ' ' || ch == '\t')) {
                split = j;
                break;
            }
        }
        
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
        
        // Detect and strip BOL anchor (^) at start of regex
        if (!rule.regex.empty() && rule.regex[0] == '^') {
            rule.bolAnchored = true;
            rule.regex = rule.regex.substr(1);
        }
        
        // Detect and strip EOL anchor ($) at end of regex (if not escaped)
        if (rule.regex.size() >= 1 && rule.regex.back() == '$') {
            // Check it's not escaped
            size_t backslashes = 0;
            if (rule.regex.size() >= 2) {
                for (int k = (int)rule.regex.size() - 2; k >= 0 && rule.regex[k] == '\\'; k--) {
                    backslashes++;
                }
            }
            if (backslashes % 2 == 0) {
                rule.eolAnchored = true;
                rule.regex.pop_back();
            }
        }
        
        _rulesList.push_back(rule);
    }
}
