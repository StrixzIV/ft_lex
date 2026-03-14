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

#include "../include/LexerParser.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

LexerParser::LexerParser(const std::vector<std::string> &filenames) : _filenames(filenames), _lineNo(1) {
    _startConditions.push_back({"INITIAL", false}); // Default start condition
}

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

    _content = "";
    int currentLine = 1;
    for (const auto &filename : _filenames) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        size_t startPos = _content.size();
        int startLine = currentLine;

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();
        _content += fileContent;

        // Count lines in this file
        for (char c : fileContent) {
            if (c == '\n')
                currentLine++;
        }

        if (!_content.empty() && _content.back() != '\n') {
            _content += '\n'; // Ensure each file ends with a newline
            currentLine++;
        }

        _fileBoundaries.push_back({filename, startLine, currentLine - 1, startPos});
    }

}

std::string LexerParser::formatError(int line, int col, const std::string &msg) const {
    std::string filename = "Lexer";
    int localLine = line;

    // Find which file this absolute line belongs to
    for (const auto &fb : _fileBoundaries) {
        if (line >= fb.startLine && line <= fb.endLine) {
            filename = fb.filename;
            localLine = line - fb.startLine + 1;
            break;
        }
    }

    std::stringstream ss;
    ss << filename << ":" << localLine << ":" << col << ": error: " << msg;
    return ss.str();
}

void LexerParser::_splitSections() {

    size_t first_sep_pos = _content.find("%%");

    if (first_sep_pos == std::string::npos) {
        throw std::runtime_error(formatError(1, 1, "Missing first '%%' delimiter"));
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

    // Calculate initial _lineNo for rules section
    _lineNo = 1;
    for (size_t i = 0; i < start_rules_pos; ++i) {
        if (_content[i] == '\n')
            _lineNo++;
    }
}

void LexerParser::parse() {
    _readFile();
    _splitSections();
    // We already calculated _lineNo for rules in _splitSections.
    // _parseDefinitions needs its own local line tracking or it can use a separate pass.
    _parseDefinitions(); 
    _parseRules();
}

const std::vector<LexerParser::Rule> &LexerParser::getRulesList() const {
    return _rulesList;
}

const std::map<std::string, std::string> &LexerParser::getNamedDefinitions() const {
    return _namedDefinitions;
}

const std::vector<LexerParser::StartCondition> &LexerParser::getStartConditions() const {
    return _startConditions;
}

const std::map<int, std::string> &LexerParser::getEofActions() const {
    return _eofActions;
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
        else {
            continue;
        }

        if (trimmed.find("%{") == 0) {
            inCodeBlock = true;
            continue;
        }
        if (trimmed.find("%}") == 0) {
            inCodeBlock = false;
            continue;
        }
        if (inCodeBlock) {
            continue;
        }

        // Skip empty lines and comment lines
        if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '#') {
            continue;
        }
            
        // Parse start condition declarations (%s, %x)
        if (trimmed[0] == '%') {
            if (trimmed.size() > 1 && (trimmed[1] == 's' || trimmed[1] == 'x')) {
                bool isExclusive = (trimmed[1] == 'x');
                std::istringstream varStream(trimmed.substr(2));
                std::string condName;
                while (varStream >> condName) {
                    _startConditions.push_back({condName, isExclusive});
                }
            }
            continue;
        }

        // Check if line looks like: NAME<whitespace>regex
        // NAME must start with letter or underscore
        if (!std::isalpha(trimmed[0]) && trimmed[0] != '_') {
            continue;
        }

        // Find the end of the name
        size_t nameEnd = 0;
        while (nameEnd < trimmed.size() && (std::isalnum(trimmed[nameEnd]) || trimmed[nameEnd] == '_'))
            nameEnd++;

        if (nameEnd >= trimmed.size()) {
            continue; // No regex after name
        }

        // Must have whitespace after name
        size_t regexStart = trimmed.find_first_not_of(" \t", nameEnd);
        if (regexStart == std::string::npos) {
            continue;
        }

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
    // We need to track the absolute position in _content to use _fileBoundaries effectively,
    // or just calculate local file positions carefully.
    // _rules is a substring of _content. Let's find its start position in _content.
    size_t rulesStartInContent = _content.find(_rules);
    if (rulesStartInContent == std::string::npos) rulesStartInContent = 0;

    size_t currentPos = rulesStartInContent;
    int currentLine = _lineNo;
    int currentCol = 1;

    auto advance = [&](size_t n) {
        for (size_t k = 0; k < n; ++k) {
            if (_content[currentPos] == '\n') {
                currentLine++;
                currentCol = 1;
            } else {
                currentCol++;
            }
            currentPos++;
        }
    };


    // Since _rules might have been modified or simplified, let's parse from _content directly
    // starting after the first %%.
    size_t firstSep = _content.find("%%");
    currentPos = firstSep + 2;
    currentLine = 1;
    for (size_t i = 0; i < currentPos; ++i) if (_content[i] == '\n') currentLine++;
    currentCol = 1; // Actually we should calculate col based on last \n

    while (currentPos < _content.size()) {
        // Check for second %%
        if (_content.substr(currentPos, 2) == "%%") {
            break;
        }

        // Skip leading whitespace of a line
        while (currentPos < _content.size() && (_content[currentPos] == ' ' || _content[currentPos] == '\t' || _content[currentPos] == '\r' || _content[currentPos] == '\n')) {
            advance(1);
            if (currentPos < _content.size() && _content.substr(currentPos, 2) == "%%") goto end_rules;
        }

        if (currentPos >= _content.size()) break;

        Rule rule;
        rule.lineNo = currentLine;
        

        // --- Extract State Prefix <STATE1,STATE2> ---
        if (_content[currentPos] == '<' && _content.substr(currentPos, 7) != "<<EOF>>") {
            size_t endAngle = _content.find('>', currentPos);
            if (endAngle != std::string::npos) {
                std::string statesStr = _content.substr(currentPos + 1, endAngle - currentPos - 1);
                std::istringstream ss(statesStr);
                std::string state;
                while (std::getline(ss, state, ',')) {
                    size_t s = state.find_first_not_of(" \t");
                    size_t e = state.find_last_not_of(" \t");
                    if (s != std::string::npos && e != std::string::npos) {
                        rule.startConditions.push_back(state.substr(s, e - s + 1));
                    }
                }
                advance(endAngle - currentPos + 1);
                // Skip space after >
                while (currentPos < _content.size() && (_content[currentPos] == ' ' || _content[currentPos] == '\t')) advance(1);
            }
        }

        if (currentPos >= _content.size()) break;

        // --- Detect <<EOF>> rule ---
        if (_content.substr(currentPos, 7) == "<<EOF>>") {
            int eofLine = currentLine;
            int eofCol = currentCol;
            advance(7);
            
            // Extract action
            while (currentPos < _content.size() && (_content[currentPos] == ' ' || _content[currentPos] == '\t')) advance(1);
            
            std::string eofAction;
            if (currentPos < _content.size() && _content[currentPos] == '{') {
                int braceDepth = 0;
                while (currentPos < _content.size()) {
                    char ac = _content[currentPos];
                    if (ac == '{') braceDepth++;
                    if (ac == '}') braceDepth--;
                    eofAction += ac;
                    advance(1);
                    if (braceDepth == 0) break;
                }
            } else {
                size_t lineEnd = _content.find('\n', currentPos);
                if (lineEnd == std::string::npos) lineEnd = _content.size();
                eofAction = _content.substr(currentPos, lineEnd - currentPos);
                advance(lineEnd - currentPos);
            }

            // Map EOF action to start conditions
            std::vector<int> condIndices;
            if (rule.startConditions.empty() || rule.startConditions[0] == "*") {
                for (size_t ci = 0; ci < _startConditions.size(); ++ci) condIndices.push_back((int)ci);
            } else {
                for (const auto &sc : rule.startConditions) {
                    bool found = false;
                    for (size_t ci = 0; ci < _startConditions.size(); ++ci) {
                        if (_startConditions[ci].name == sc) {
                            condIndices.push_back((int)ci);
                            found = true;
                        }
                    }
                    if (!found) {
                        std::cerr << formatError(eofLine, eofCol, "unknown start condition '" + sc + "'") << "\n";
                    }
                }
            }
            for (int ci : condIndices) _eofActions[ci] = eofAction;
            continue;
        }

        // --- Extract Regex ---
        size_t regexStartPos = currentPos;
        
        

        while (currentPos < _content.size()) {
            char c = _content[currentPos];
            if (c == ' ' || c == '\t' || c == '\n') break;
            if (c == '\\') { advance(2); continue; }
            if (c == '[') {
                advance(1);
                if (currentPos < _content.size() && _content[currentPos] == '^') advance(1);
                if (currentPos < _content.size() && _content[currentPos] == ']') advance(1);
                while (currentPos < _content.size() && _content[currentPos] != ']') {
                    if (_content[currentPos] == '\\') advance(1);
                    advance(1);
                }
                advance(1);
                continue;
            }
            if (c == '"') {
                advance(1);
                while (currentPos < _content.size() && _content[currentPos] != '"') {
                    if (_content[currentPos] == '\\') advance(1);
                    advance(1);
                }
                advance(1);
                continue;
            }
            advance(1);
        }

        rule.regex = _content.substr(regexStartPos, currentPos - regexStartPos);
        if (rule.regex.empty()) continue;
        rule.regex = _expandDefinitions(rule.regex);

        // --- Extract Action ---
        while (currentPos < _content.size() && (_content[currentPos] == ' ' || _content[currentPos] == '\t')) advance(1);

        if (currentPos < _content.size() && _content[currentPos] == '{') {
            int braceDepth = 0;
            size_t actionStart = currentPos;
            while (currentPos < _content.size()) {
                char ac = _content[currentPos];
                if (ac == '"' || ac == '\'') {
                    char q = ac; advance(1);
                    while (currentPos < _content.size() && _content[currentPos] != q) {
                        if (_content[currentPos] == '\\') advance(1);
                        advance(1);
                    }
                    advance(1); continue;
                }
                if (ac == '{') braceDepth++;
                if (ac == '}') braceDepth--;
                advance(1);
                if (braceDepth == 0) break;
            }
            rule.action = _content.substr(actionStart, currentPos - actionStart);
        } else {
            size_t lineEnd = _content.find('\n', currentPos);
            if (lineEnd == std::string::npos) lineEnd = _content.size();
            rule.action = _content.substr(currentPos, lineEnd - currentPos);
            advance(lineEnd - currentPos);
        }

        _rulesList.push_back(rule);
    }

end_rules:;
}
