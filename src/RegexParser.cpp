/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:53 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:12:53 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RegexParser.hpp"
#include <stack>
#include <iostream>
#include <stdexcept>

std::vector<Token> RegexParser::toPostfix(const std::string &regex) {

    std::vector<Token> tokens = _tokenize(regex);
    std::vector<Token> explicitConcat = _addExplicitConcat(tokens);
    std::vector<Token> postfix;
    std::stack<Token> operators;

    for (const auto &token : explicitConcat) {
        if (token.type == CHAR || token.type == CHARSET) {
            postfix.push_back(token);
        } else if (token.type == OPERATOR) {
            if (token.c == '(') {
                operators.push(token);
            } else if (token.c == ')') {
                while (!operators.empty() && operators.top().c != '(') {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                if (!operators.empty()) {
                    operators.pop(); // Pop '('
                }
            } else {
                while (!operators.empty() && operators.top().c != '(' && 
                       _getPrecedence(operators.top()) >= _getPrecedence(token)) {
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

std::vector<Token> RegexParser::_tokenize(const std::string &regex) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < regex.size(); ++i) {
        char c = regex[i];

        if (c == '\\') {
            if (i + 1 < regex.size()) {
                char next = regex[++i];
                char escaped = next;
                
                if (next == 'n') {
                    escaped = '\n';
                } else if (next == 't') {
                    escaped = '\t';
                } else if (next == 'r') {
                    escaped = '\r';
                } else if (next == 'v') {
                    escaped = '\v';
                } else if (next == 'f') {
                    escaped = '\f';
                }
                // else keep literal (e.g. \. -> .)
                
                tokens.push_back(Token(escaped, CHAR));
            } else {
                throw std::runtime_error("Trailing backslash in regex");
            }
        } else if (c == '[') {
            // Parse character set
            std::set<char> set;
            i++;
            // TODO: Handle negation [^...]
            bool first = true;
            size_t start = i;
            
            // We need to parse content manually to handle escapes inside []
            while (i < regex.size() && (regex[i] != ']' || first)) {
                if (regex[i] == '\\') {
                     i++; // Skip backslash
                     if (i >= regex.size()) {
                         throw std::runtime_error("Trailing backslash in class");
                     }
                }
                first = false;
                i++;
            }
            if (i == regex.size()) {
                throw std::runtime_error("Unmatched [");
            }
            
            // Re-scan the content to build the set
            std::string rawContent = regex.substr(start, i - start);
            
            // Now parse rawContent handling escapes and ranges
            for (size_t j = 0; j < rawContent.size(); ++j) {
                char current;
                
                // Handle escape
                if (rawContent[j] == '\\') {
                    if (j + 1 >= rawContent.size()) {
                        break; // Should not happen
                    }
                    char next = rawContent[++j];
                    
                    if (next == 'n') {
                        current = '\n';
                    } else if (next == 't') {
                        current = '\t';
                    } else if (next == 'r') {
                        current = '\r';
                    } else if (next == 'v') {
                        current = '\v';
                    } else if (next == 'f') {
                        current = '\f';
                    } else {
                        current = next;
                    }
                } else {
                    current = rawContent[j];
                }
                
                // Check for range
                if (j + 2 < rawContent.size() && rawContent[j+1] == '-') {
                    // This is tricky because we need to know the END of the range
                    // which might also be escaped.
                    // Let's look ahead
                    size_t nextIdx = j + 2;
                    char rangeEnd;
                    
                     if (rawContent[nextIdx] == '\\') {
                        if (nextIdx + 1 >= rawContent.size()) {
                            break; // Error
                        }
                        char next = rawContent[++nextIdx];
                        
                        if (next == 'n') {
                            rangeEnd = '\n';
                        } else if (next == 't') {
                            rangeEnd = '\t';
                        } else if (next == 'r') {
                            rangeEnd = '\r';
                        } else if (next == 'v') {
                            rangeEnd = '\v';
                        } else if (next == 'f') {
                            rangeEnd = '\f';
                        } else {
                            rangeEnd = next;
                        }
                    } else {
                        rangeEnd = rawContent[nextIdx];
                    }
                    
                    for (char rc = current; rc <= rangeEnd; ++rc) {
                        set.insert(rc);
                    }
                    j = nextIdx; // Advance to end of range
                } else {
                    set.insert(current);
                }
            }
            tokens.push_back(Token(set));
        } else if (c == '(' || c == ')' || c == '*' || c == '+' || c == '?' || c == '|') {
            tokens.push_back(Token(c, OPERATOR));
        } else if (c == '.') {
             // Dot is a special charset (all except \n)
             std::set<char> dotSet;
             for (int k = -128; k <= 127; ++k) {
                 if ((char)k != '\n') {
                     dotSet.insert((char)k);
                 }
             }
             tokens.push_back(Token(dotSet));
        } else {
            tokens.push_back(Token(c, CHAR));
        }
    }
    return tokens;
}

std::vector<Token> RegexParser::_addExplicitConcat(const std::vector<Token> &tokens) {
    std::vector<Token> result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        result.push_back(tokens[i]);

        if (i + 1 < tokens.size()) {
            const Token &curr = tokens[i];
            const Token &next = tokens[i+1];
            
            bool currIsOperand = (curr.type == CHAR || curr.type == CHARSET || (curr.type == OPERATOR && (curr.c == ')' || curr.c == '*' || curr.c == '+' || curr.c == '?')));
            bool nextIsOperand = (next.type == CHAR || next.type == CHARSET || (next.type == OPERATOR && next.c == '('));

            if (currIsOperand && nextIsOperand) {
                result.push_back(Token(CONCAT_OP, OPERATOR));
            }
        }
    }
    return result;
}

int RegexParser::_getPrecedence(const Token &t) {
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
