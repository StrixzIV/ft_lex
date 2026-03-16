/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:53 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/15 18:36:57 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RegexParser.hpp"

#include <cctype>
#include <iostream>
#include <stack>
#include <stdexcept>

std::vector<Token> RegexParser::toPostfix(const std::string &regex) {

    std::vector<Token> tokens = _tokenize(regex);
    std::vector<Token> explicitConcat = _addExplicitConcat(tokens);
    std::vector<Token> postfix;
    std::stack<Token> operators;

    for (const auto &token : explicitConcat) {
        if (token.type == CHAR || token.type == CHARSET ||
            token.type == ANCHOR_START || token.type == ANCHOR_END) {
            postfix.push_back(token);
        } else if (token.type == INTERVAL) {
            postfix.push_back(token);
        } else if (token.type == TRAILING_CONTEXT_OP) {

            while (!operators.empty() && operators.top().c != '(' &&
                   _getPrecedence(operators.top()) >= _getPrecedence(token)) {
                postfix.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        } else if (token.type == OPERATOR) {
            if (token.c == '(') {
                operators.push(token);
            } else if (token.c == ')') {
                while (!operators.empty() && operators.top().c != '(') {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                if (!operators.empty()) {
                    operators.pop();
                }
            } else {
                while (!operators.empty() && operators.top().c != '(' &&
                       _getPrecedence(operators.top()) >=
                           _getPrecedence(token)) {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
        }
    }

    while (!operators.empty()) {
        postfix.push_back(operators.top());
        operators.pop();
    }

    return postfix;
}

static std::set<int> _getPOSIXClass(const std::string &name) {
    std::set<int> s;
    for (int c = 0; c < 256; ++c) {
        bool match = false;
        if (name == "alpha") match = isalpha((unsigned char)c);
        else if (name == "upper") match = isupper((unsigned char)c);
        else if (name == "lower") match = islower((unsigned char)c);
        else if (name == "digit") match = isdigit((unsigned char)c);
        else if (name == "alnum") match = isalnum((unsigned char)c);
        else if (name == "space") match = isspace((unsigned char)c);
        else if (name == "blank") match = isblank((unsigned char)c);
        else if (name == "print") match = isprint((unsigned char)c);
        else if (name == "graph") match = isgraph((unsigned char)c);
        else if (name == "cntrl") match = iscntrl((unsigned char)c);
        else if (name == "xdigit") match = isxdigit((unsigned char)c);
        else if (name == "punct") match = ispunct((unsigned char)c);
        
        if (match) s.insert(c);
    }
    return s;
}

std::vector<Token> RegexParser::_tokenize(const std::string &regex) {
    std::vector<Token> tokens;

    size_t len = regex.size();

    for (size_t i = 0; i < len; ++i) {
        char c = regex[i];

        if (i == 0 && c == '^') {
            tokens.push_back(Token('^', ANCHOR_START));
            continue;
        }
        if (i == len - 1 && c == '$') {

            int bsCount = 0;
            for (int k = (int)i - 1; k >= 0 && regex[k] == '\\'; --k) {
                bsCount++;
            }
            if (bsCount % 2 == 0) {
                tokens.push_back(Token('$', ANCHOR_END));
                continue;
            }
        }

        if (c == '\\') {
            if (i + 1 < regex.size()) {
                char next = regex[++i];
                char escaped = next;

                if (next == 'n')
                    escaped = '\n';
                else if (next == 't')
                    escaped = '\t';
                else if (next == 'r')
                    escaped = '\r';
                else if (next == 'v')
                    escaped = '\v';
                else if (next == 'f')
                    escaped = '\f';

                tokens.push_back(Token(escaped, CHAR));
            } else {
                throw std::runtime_error("Trailing backslash in regex");
            }
        } else if (c == '[') {

            std::set<int> set;
            i++;
            bool negated = false;
            if (i < regex.size() && regex[i] == '^') {
                negated = true;
                i++;
            }

            bool first = true;
            size_t start = i;

            while (i < regex.size() && (regex[i] != ']' || first)) {
                if (regex[i] == '\\') {
                    i++;
                    if (i >= regex.size())
                        throw std::runtime_error("Trailing backslash in class");
                } else if (regex[i] == '[' && i + 1 < regex.size() && (regex[i+1] == ':' || regex[i+1] == '.' || regex[i+1] == '=')) {
                    
                    char type = regex[i+1];
                    
                    i += 2;
                    
                    while (i + 1 < regex.size() && !(regex[i] == type && regex[i+1] == ']')) {
                        i++;
                    }
                    
                    if (i + 1 < regex.size()) i++;

                }
                first = false;
                i++;
            }
            if (i == regex.size())
                throw std::runtime_error("Unmatched [");

            std::string rawContent = regex.substr(start, i - start);

            for (size_t j = 0; j < rawContent.size(); ++j) {

                if (rawContent.substr(j, 2) == "[:") {
                    size_t end = rawContent.find(":]", j + 2);
                    if (end != std::string::npos) {
                        std::string name =
                            rawContent.substr(j + 2, end - (j + 2));
                        std::set<int> cls = _getPOSIXClass(name);
                        set.insert(cls.begin(), cls.end());
                        j = end + 1;
                        continue;
                    }
                }

                if (rawContent.substr(j, 2) == "[.") {
                    size_t end = rawContent.find(".]", j + 2);
                    if (end != std::string::npos) {
                        std::string content = rawContent.substr(j + 2, end - (j + 2));
                        if (content.size() == 1) {
                            set.insert((unsigned char)content[0]);
                        } else {
                            std::cerr << "warning: multi-character collating elements not supported: [." << content << ".]" << std::endl;
                        }
                        j = end + 1;
                        continue;
                    }
                }

                if (rawContent.substr(j, 2) == "[=") {
                    size_t end = rawContent.find("=]", j + 2);
                    if (end != std::string::npos) {
                        std::string content = rawContent.substr(j + 2, end - (j + 2));
                        if (content.size() == 1) {
                            set.insert((unsigned char)content[0]);
                        } else {
                            std::cerr << "warning: multi-character equivalence classes not supported: [=" << content << "=]" << std::endl;
                        }
                        j = end + 1;
                        continue;
                    }
                }

                char current;
                if (rawContent[j] == '\\' && j + 1 < rawContent.size()) {
                    char next = rawContent[++j];
                    if (next == 'n')
                        current = '\n';
                    else if (next == 't')
                        current = '\t';
                    else if (next == 'r')
                        current = '\r';
                    else if (next == 'v')
                        current = '\v';
                    else if (next == 'f')
                        current = '\f';
                    else
                        current = next;
                } else {
                    current = rawContent[j];
                }

                if (j + 2 < rawContent.size() && rawContent[j + 1] == '-') {
                    size_t nextIdx = j + 2;
                    char rangeEnd;
                    if (rawContent[nextIdx] == '\\' &&
                        nextIdx + 1 < rawContent.size()) {
                        char next = rawContent[++nextIdx];
                        if (next == 'n')
                            rangeEnd = '\n';
                        else if (next == 't')
                            rangeEnd = '\t';
                        else if (next == 'r')
                            rangeEnd = '\r';
                        else if (next == 'v')
                            rangeEnd = '\v';
                        else if (next == 'f')
                            rangeEnd = '\f';
                        else
                            rangeEnd = next;
                    } else {
                        rangeEnd = rawContent[nextIdx];
                    }
                    for (int rc = (unsigned char)current;
                         rc <= (unsigned char)rangeEnd; ++rc)
                        set.insert(rc);
                    j = nextIdx;
                } else {
                    set.insert((unsigned char)current);
                }
            }

            if (negated) {
                std::set<int> inverted;
                for (int k = 0; k < 256; ++k) {
                    if (set.find(k) == set.end())
                        inverted.insert(k);
                }
                tokens.push_back(Token(inverted));
            } else {
                tokens.push_back(Token(set));
            }
        } else if (c == '{') {

            size_t close = regex.find('}', i);
            if (close != std::string::npos && !tokens.empty()) {
                std::string content = regex.substr(i + 1, close - i - 1);
                bool valid = true;
                int n = -1, m = -1;
                size_t comma = content.find(',');
                try {
                    if (comma == std::string::npos) {
                        n = std::stoi(content);
                        m = n;
                    } else {
                        n = std::stoi(content.substr(0, comma));
                        if (comma + 1 < content.size())
                            m = std::stoi(content.substr(comma + 1));
                        else
                            m = -1;
                    }
                } catch (...) {
                    valid = false;
                }

                if (valid && n >= 0) {

                    tokens.push_back(Token(n, m));
                    i = close;
                    continue;
                }
            }
            tokens.push_back(Token('{', CHAR));
        } else if (c == '"') {

            i++;
            while (i < regex.size() && regex[i] != '"') {
                if (regex[i] == '\\') {
                    i++;
                    if (i >= regex.size())
                        throw std::runtime_error("Trailing backslash in quote");
                    char next = regex[i];
                    char escaped = next;
                    if (next == 'n')
                        escaped = '\n';
                    else if (next == 't')
                        escaped = '\t';
                    else if (next == 'r')
                        escaped = '\r';
                    else if (next == 'v')
                        escaped = '\v';
                    else if (next == 'f')
                        escaped = '\f';
                    tokens.push_back(Token(escaped, CHAR));
                } else {
                    tokens.push_back(Token(regex[i], CHAR));
                }
                i++;
            }
            if (i == regex.size())
                throw std::runtime_error("Unmatched \"");
        } else if (c == '(' || c == ')' || c == '*' || c == '+' || c == '?' ||
                   c == '|') {
            tokens.push_back(Token(c, OPERATOR));
        } else if (c == '/') {

            tokens.push_back(Token('/', TRAILING_CONTEXT_OP));
        } else if (c == '.') {

            std::set<int> dotSet;
            for (int k = 0; k < 256; ++k) {
                if (k != '\n') {
                    dotSet.insert(k);
                }
            }
            tokens.push_back(Token(dotSet));
        } else {
            tokens.push_back(Token(c, CHAR));
        }
    }
    return tokens;
}

std::vector<Token>
RegexParser::_addExplicitConcat(const std::vector<Token> &tokens) {
    std::vector<Token> result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        result.push_back(tokens[i]);

        if (i + 1 < tokens.size()) {
            const Token &curr = tokens[i];
            const Token &next = tokens[i + 1];

            bool currIsOperand =
                (curr.type == CHAR || curr.type == CHARSET ||
                 curr.type == ANCHOR_START || curr.type == ANCHOR_END ||
                 (curr.type == OPERATOR && (curr.c == ')' || curr.c == '*' ||
                                            curr.c == '+' || curr.c == '?')));

            bool nextIsOperand = (next.type == CHAR || next.type == CHARSET ||
                                  next.type == ANCHOR_END ||
                                  (next.type == OPERATOR && next.c == '('));

            bool nextIsAnchorStart = (next.type == ANCHOR_START);

            if (currIsOperand && (nextIsOperand || nextIsAnchorStart)) {
                result.push_back(Token(CONCAT_OP, OPERATOR));
            }
        }
    }
    return result;
}

int RegexParser::_getPrecedence(const Token &t) {
    if (t.type == TRAILING_CONTEXT_OP)
        return 0;
    if (t.type == INTERVAL) {
        return 3;
    }
    if (t.type != OPERATOR) {
        return 0;
    }
    char c = t.c;
    if (c == '*' || c == '+' || c == '?') {
        return 3;
    }
    if (c == CONCAT_OP) {
        return 2;
    }
    if (c == '|') {
        return 1;
    }
    return 0;
}
