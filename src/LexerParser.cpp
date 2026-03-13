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

    std::string rawDefs = _content.substr(0, first_sep_pos);
    _rawDefinitions = rawDefs;

    // Extract only C code blocks (content between %{ and %}) for the header
    _definitions = "";
    size_t searchPos = 0;
    while (searchPos < rawDefs.size()) {
        size_t openPos = rawDefs.find("%{", searchPos);
        if (openPos == std::string::npos)
            break;
        size_t closePos = rawDefs.find("%}", openPos + 2);
        if (closePos == std::string::npos)
            break;
        // Content between %{ and %}
        _definitions += rawDefs.substr(openPos + 2, closePos - openPos - 2);
        searchPos = closePos + 2;
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

const std::map<std::string, std::string> &LexerParser::getNamedDefinitions() const {
    return _namedDefinitions;
}

void LexerParser::_parseDefinitions() {
    // Parse lines in the definitions section that are NOT inside %{ %} blocks.
    // Lines of the form:  NAME  regex
    // where NAME starts with a letter or underscore.
    std::istringstream stream(_rawDefinitions);
    std::string line;
    bool inCodeBlock = false;

    while (std::getline(stream, line)) {
        // Check for %{ / %} markers
        std::string trimmed = line;
        size_t first = trimmed.find_first_not_of(" \t\r\n");
        if (first != std::string::npos)
            trimmed = trimmed.substr(first);
        else
            continue;

        if (trimmed.find("%{") == 0) {
            inCodeBlock = true;
            continue;
        }
        if (trimmed.find("%}") == 0) {
            inCodeBlock = false;
            continue;
        }
        if (inCodeBlock)
            continue;

        // Skip empty lines and comment lines
        if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '#')
            continue;
        // Skip start condition declarations (%s, %x)
        if (trimmed[0] == '%')
            continue;

        // Check if line looks like: NAME<whitespace>regex
        // NAME must start with letter or underscore
        if (!std::isalpha(trimmed[0]) && trimmed[0] != '_')
            continue;

        // Find the end of the name
        size_t nameEnd = 0;
        while (nameEnd < trimmed.size() && (std::isalnum(trimmed[nameEnd]) || trimmed[nameEnd] == '_'))
            nameEnd++;

        if (nameEnd >= trimmed.size())
            continue; // No regex after name

        // Must have whitespace after name
        size_t regexStart = trimmed.find_first_not_of(" \t", nameEnd);
        if (regexStart == std::string::npos)
            continue;

        std::string name = trimmed.substr(0, nameEnd);
        std::string regex = trimmed.substr(regexStart);

        _namedDefinitions[name] = regex;
    }
}

std::string LexerParser::_expandDefinitions(const std::string &regex) {
    std::string result = regex;
    // Iteratively expand {NAME} references (supports recursive definitions)
    bool changed = true;
    int maxIterations = 100; // prevent infinite loops from circular definitions

    while (changed && maxIterations-- > 0) {
        changed = false;
        size_t pos = 0;
        while ((pos = result.find('{', pos)) != std::string::npos) {
            // Check if this is inside a character class [...]
            // Simple heuristic: skip if preceded by unmatched [
            // Actually for lex, {NAME} outside [...] is a definition reference
            // Inside [...] it's literal. But the regex tokenizer handles [...] separately.
            // So we just expand all {NAME} that match a known definition.

            size_t closePos = result.find('}', pos);
            if (closePos == std::string::npos)
                break;

            std::string name = result.substr(pos + 1, closePos - pos - 1);

            auto it = _namedDefinitions.find(name);
            if (it != _namedDefinitions.end()) {
                // Replace {NAME} with (regex) — wrapped in parens for grouping safety
                std::string replacement = "(" + it->second + ")";
                result = result.substr(0, pos) + replacement + result.substr(closePos + 1);
                changed = true;
                pos += replacement.size(); // advance past the replacement
            } else {
                pos = closePos + 1;
            }
        }
    }
    return result;
}

void LexerParser::_parseRules() {
    std::istringstream stream(_rules);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty / whitespace-only lines
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            continue;
        
        std::string trimmed = line.substr(first);
        if (trimmed.empty())
            continue;

        // --- Extract the regex pattern ---
        // We must skip over [...] character classes and "..." quoted strings
        // because they can contain spaces.
        size_t i = 0;
        while (i < trimmed.size()) {
            char c = trimmed[i];

            // Whitespace outside brackets/quotes = end of pattern
            if (c == ' ' || c == '\t')
                break;

            if (c == '\\') {
                // Escaped char — skip next
                i += 2;
                continue;
            }

            if (c == '[') {
                // Character class — scan to matching ']'
                i++;
                if (i < trimmed.size() && trimmed[i] == '^')
                    i++;  // negation
                if (i < trimmed.size() && trimmed[i] == ']')
                    i++;  // literal ] at start
                while (i < trimmed.size() && trimmed[i] != ']') {
                    if (trimmed[i] == '\\')
                        i++;  // skip escaped char inside class
                    i++;
                }
                // i now points at ']' or end
                i++;
                continue;
            }

            if (c == '"') {
                // Quoted string — scan to matching '"'
                i++;
                while (i < trimmed.size() && trimmed[i] != '"') {
                    if (trimmed[i] == '\\')
                        i++;
                    i++;
                }
                // i now points at closing '"' or end
                i++;
                continue;
            }

            i++;
        }
        
        Rule rule;
        rule.regex = trimmed.substr(0, i);
        
        if (rule.regex.empty())
            continue;

        // Expand named definitions before storing
        rule.regex = _expandDefinitions(rule.regex);

        // --- Extract the action ---
        // Skip whitespace between pattern and action
        size_t actionStart = trimmed.find_first_not_of(" \t", i);
        
        if (actionStart == std::string::npos) {
            // No action on this line
            rule.action = "";
        } else if (trimmed[actionStart] == '{') {
            // Brace-counted multi-line action
            int braceDepth = 0;
            std::string actionText;
            std::string actionLine = trimmed.substr(actionStart);
            
            while (true) {
                for (size_t j = 0; j < actionLine.size(); ++j) {
                    char ac = actionLine[j];
                    
                    // Skip string literals
                    if (ac == '"' || ac == '\'') {
                        char quote = ac;
                        actionText += ac;
                        j++;
                        while (j < actionLine.size() && actionLine[j] != quote) {
                            if (actionLine[j] == '\\') {
                                actionText += actionLine[j++];
                                if (j < actionLine.size())
                                    actionText += actionLine[j];
                            } else {
                                actionText += actionLine[j];
                            }
                            j++;
                        }
                        if (j < actionLine.size())
                            actionText += actionLine[j]; // closing quote
                        continue;
                    }
                    
                    // Skip C comments
                    if (ac == '/' && j + 1 < actionLine.size()) {
                        if (actionLine[j + 1] == '/') {
                            // Line comment — rest of line is comment
                            actionText += actionLine.substr(j);
                            j = actionLine.size();
                            break;
                        }
                        if (actionLine[j + 1] == '*') {
                            // Block comment — scan until */
                            actionText += "/*";
                            j += 2;
                            while (j < actionLine.size()) {
                                if (actionLine[j] == '*' && j + 1 < actionLine.size() && actionLine[j + 1] == '/') {
                                    actionText += "*/";
                                    j++;
                                    break;
                                }
                                actionText += actionLine[j];
                                j++;
                            }
                            continue;
                        }
                    }
                    
                    if (ac == '{') braceDepth++;
                    if (ac == '}') braceDepth--;
                    actionText += ac;
                    
                    if (braceDepth == 0) {
                        // Done — action complete
                        break;
                    }
                }
                
                if (braceDepth == 0)
                    break;
                
                // Need more lines
                if (!std::getline(stream, line))
                    break;
                actionText += '\n';
                actionLine = line;
            }
            
            rule.action = actionText;
        } else {
            // Single expression action (no braces) — rest of line
            rule.action = trimmed.substr(actionStart);
        }
        
        _rulesList.push_back(rule);
    }
}
