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
#include <iostream>
#include <sstream>

LexerParser::LexerParser(const std::vector<std::string> &filenames)
    : _filenames(filenames), _lineNo(1), _usePointerMode(false) {
    _startConditions.push_back({"INITIAL", false});
}

bool LexerParser::isPointerMode() const { return _usePointerMode; }

const std::string &LexerParser::getDefinitions() const { return _definitions; }

const std::string &LexerParser::getRules() const { return _rules; }

const std::string &LexerParser::getUserCode() const { return _userCode; }

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

        for (char c : fileContent) {
            if (c == '\n')
                currentLine++;
        }

        if (!_content.empty() && _content.back() != '\n') {
            _content += '\n';
            currentLine++;
        }

        _fileBoundaries.push_back(
            {filename, startLine, currentLine - 1, startPos});
    }
}

std::string LexerParser::formatError(int line, int col,
                                     const std::string &msg) const {
    std::string filename = "Lexer";
    int localLine = line;

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
        throw std::runtime_error(
            formatError(1, 1, "Missing first '%%' delimiter"));
    }

    std::string rawDefs = _content.substr(0, first_sep_pos);
    _rawDefinitions = rawDefs;

    _definitions = "";
    size_t searchPos = 0;
    while (searchPos < rawDefs.size()) {
        size_t openPos = rawDefs.find("%{", searchPos);
        if (openPos == std::string::npos)
            break;
        size_t closePos = rawDefs.find("%}", openPos + 2);
        if (closePos == std::string::npos)
            break;

        _definitions += rawDefs.substr(openPos + 2, closePos - openPos - 2);
        searchPos = closePos + 2;
    }

    size_t start_rules_pos = first_sep_pos + 2;
    size_t second_sep_pos = _content.find("%%", start_rules_pos);

    if (second_sep_pos == std::string::npos) {
        _rules = _content.substr(start_rules_pos);
        _userCode = "";
    } else {
        _rules =
            _content.substr(start_rules_pos, second_sep_pos - start_rules_pos);
        _userCode = _content.substr(second_sep_pos + 2);
    }

    _lineNo = 1;
    for (size_t i = 0; i < start_rules_pos; ++i) {
        if (_content[i] == '\n')
            _lineNo++;
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

const std::map<std::string, std::string> &
LexerParser::getNamedDefinitions() const {
    return _namedDefinitions;
}

const std::vector<LexerParser::StartCondition> &
LexerParser::getStartConditions() const {
    return _startConditions;
}

const std::map<int, std::string> &LexerParser::getEofActions() const {
    return _eofActions;
}

void LexerParser::_parseDefinitions() {

    std::istringstream stream(_rawDefinitions);
    std::string line;
    bool inCodeBlock = false;

    while (std::getline(stream, line)) {

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

        if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '#') {
            continue;
        }

        if (trimmed[0] == '%') {
            if (trimmed.find("%s") == 0 || trimmed.find("%x") == 0) {
                bool isExclusive = (trimmed[1] == 'x');
                std::istringstream varStream(trimmed.substr(2));
                std::string condName;
                while (varStream >> condName) {
                    _startConditions.push_back({condName, isExclusive});
                }
            } else if (trimmed.find("%array") == 0) {
                _usePointerMode = false;
            } else if (trimmed.find("%pointer") == 0) {
                _usePointerMode = true;
            } else if (trimmed.find("%option") == 0) {
                std::cerr << "warning: %option ignored" << std::endl;
            } else {
                size_t spacePos = trimmed.find_first_of(" \t");
                std::string directive = (spacePos == std::string::npos) ? trimmed : trimmed.substr(0, spacePos);
                std::cerr << "warning: unknown directive '" << directive << "' ignored" << std::endl;
            }
            continue;
        }

        if (!std::isalpha(trimmed[0]) && trimmed[0] != '_') {
            continue;
        }

        size_t nameEnd = 0;
        while (nameEnd < trimmed.size() &&
               (std::isalnum(trimmed[nameEnd]) || trimmed[nameEnd] == '_'))
            nameEnd++;

        if (nameEnd >= trimmed.size()) {
            continue;
        }

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

    bool changed = true;
    int maxIterations = 100;

    while (changed && maxIterations-- > 0) {
        changed = false;
        size_t pos = 0;
        while ((pos = result.find('{', pos)) != std::string::npos) {

            size_t closePos = result.find('}', pos);
            if (closePos == std::string::npos)
                break;

            std::string name = result.substr(pos + 1, closePos - pos - 1);

            auto it = _namedDefinitions.find(name);
            if (it != _namedDefinitions.end()) {

                std::string replacement = "(" + it->second + ")";
                result = result.substr(0, pos) + replacement +
                         result.substr(closePos + 1);
                changed = true;
                pos += replacement.size();
            } else {
                pos = closePos + 1;
            }
        }
    }
    return result;
}

void LexerParser::_parseRules() {

    size_t rulesStartInContent = _content.find(_rules);
    if (rulesStartInContent == std::string::npos)
        rulesStartInContent = 0;

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

    size_t firstSep = _content.find("%%");
    currentPos = firstSep + 2;
    currentLine = 1;
    for (size_t i = 0; i < currentPos; ++i)
        if (_content[i] == '\n')
            currentLine++;
    currentCol = 1;

    while (currentPos < _content.size()) {

        if (_content.substr(currentPos, 2) == "%%") {
            break;
        }

        while (currentPos < _content.size() &&
               (_content[currentPos] == ' ' || _content[currentPos] == '\t' ||
                _content[currentPos] == '\r' || _content[currentPos] == '\n')) {
            advance(1);
            if (currentPos < _content.size() &&
                _content.substr(currentPos, 2) == "%%")
                break;
        }

        if (currentPos >= _content.size() ||
            _content.substr(currentPos, 2) == "%%")
            break;

        Rule rule;
        rule.lineNo = currentLine;

        if (_content[currentPos] == '<' &&
            _content.substr(currentPos, 7) != "<<EOF>>") {
            size_t endAngle = _content.find('>', currentPos);
            if (endAngle != std::string::npos) {
                std::string statesStr =
                    _content.substr(currentPos + 1, endAngle - currentPos - 1);
                std::istringstream ss(statesStr);
                std::string state;
                while (std::getline(ss, state, ',')) {
                    size_t s = state.find_first_not_of(" \t");
                    size_t e = state.find_last_not_of(" \t");
                    if (s != std::string::npos && e != std::string::npos) {
                        std::string scName = state.substr(s, e - s + 1);
                        if (scName != "*") {
                            bool found = false;
                            for (const auto &sc : _startConditions) {
                                if (sc.name == scName) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                                throw std::runtime_error(formatError(
                                    currentLine, currentCol + (int)s + 1,
                                    "unknown start condition '" + scName +
                                        "'"));
                        }
                        rule.startConditions.push_back(scName);
                    }
                }
                advance(endAngle - currentPos + 1);

                while (currentPos < _content.size() &&
                       (_content[currentPos] == ' ' ||
                        _content[currentPos] == '\t'))
                    advance(1);
            }
        }

        if (currentPos >= _content.size())
            break;

        if (_content.substr(currentPos, 7) == "<<EOF>>") {
            int eofLine = currentLine;
            int eofCol = currentCol;
            advance(7);

            while (
                currentPos < _content.size() &&
                (_content[currentPos] == ' ' || _content[currentPos] == '\t'))
                advance(1);

            std::string eofAction;
            if (currentPos < _content.size() && _content[currentPos] == '{') {
                int braceDepth = 0;
                while (currentPos < _content.size()) {
                    char ac = _content[currentPos];
                    if (ac == '{')
                        braceDepth++;
                    if (ac == '}')
                        braceDepth--;
                    eofAction += ac;
                    advance(1);
                    if (braceDepth == 0)
                        break;
                }
            } else {
                size_t lineEnd = _content.find('\n', currentPos);
                if (lineEnd == std::string::npos)
                    lineEnd = _content.size();
                eofAction = _content.substr(currentPos, lineEnd - currentPos);
                advance(lineEnd - currentPos);
            }

            std::vector<int> condIndices;
            if (rule.startConditions.empty() ||
                rule.startConditions[0] == "*") {
                for (size_t ci = 0; ci < _startConditions.size(); ++ci)
                    condIndices.push_back((int)ci);
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
                        std::cerr << formatError(eofLine, eofCol,
                                                 "unknown start condition '" +
                                                     sc + "'")
                                  << "\n";
                    }
                }
            }
            for (int ci : condIndices)
                _eofActions[ci] = eofAction;
            continue;
        }

        size_t regexStartPos = currentPos;

        while (currentPos < _content.size()) {
            char c = _content[currentPos];
            if (c == ' ' || c == '\t' || c == '\n')
                break;
            if (c == '\\') {
                advance(2);
                continue;
            }
            if (c == '[') {
                advance(1);
                if (currentPos < _content.size() && _content[currentPos] == '^')
                    advance(1);
                if (currentPos < _content.size() && _content[currentPos] == ']')
                    advance(1);
                while (currentPos < _content.size() &&
                       _content[currentPos] != ']') {
                    if (_content[currentPos] == '\\')
                        advance(1);
                    advance(1);
                }
                advance(1);
                continue;
            }
            if (c == '"') {
                advance(1);
                while (currentPos < _content.size() &&
                       _content[currentPos] != '"') {
                    if (_content[currentPos] == '\\')
                        advance(1);
                    advance(1);
                }
                advance(1);
                continue;
            }
            advance(1);
        }

        rule.regex = _content.substr(regexStartPos, currentPos - regexStartPos);
        if (rule.regex.empty())
            continue;
        rule.hasBOL = (rule.regex[0] == '^');
        rule.regex = _expandDefinitions(rule.regex);

        while (currentPos < _content.size() &&
               (_content[currentPos] == ' ' || _content[currentPos] == '\t'))
            advance(1);

        if (currentPos < _content.size() && _content[currentPos] == '{') {
            int braceDepth = 0;
            size_t actionStart = currentPos;
            while (currentPos < _content.size()) {
                char ac = _content[currentPos];
                if (ac == '"' || ac == '\'') {
                    char q = ac;
                    advance(1);
                    while (currentPos < _content.size() &&
                           _content[currentPos] != q) {
                        if (_content[currentPos] == '\\')
                            advance(1);
                        advance(1);
                    }
                    advance(1);
                    continue;
                }
                if (ac == '{')
                    braceDepth++;
                if (ac == '}')
                    braceDepth--;
                advance(1);
                if (braceDepth == 0)
                    break;
            }
            rule.action =
                _content.substr(actionStart, currentPos - actionStart);
        } else {
            size_t lineEnd = _content.find('\n', currentPos);
            if (lineEnd == std::string::npos)
                lineEnd = _content.size();
            rule.action = _content.substr(currentPos, lineEnd - currentPos);
            advance(lineEnd - currentPos);
        }

        _rulesList.push_back(rule);

        while (currentPos < _content.size() && _content[currentPos] != '\n') {
            advance(1);
        }
    }

    for (int i = (int)_rulesList.size() - 2; i >= 0; --i) {
        std::string trimmedAction = _rulesList[i].action;
        size_t s = trimmedAction.find_first_not_of(" \t\n\r");
        size_t e = trimmedAction.find_last_not_of(" \t\n\r");
        if (s != std::string::npos && e != std::string::npos) {
            trimmedAction = trimmedAction.substr(s, e - s + 1);
        }
        if (trimmedAction == "|") {
            _rulesList[i].action = _rulesList[i + 1].action;
        }
    }
}
