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

std::string RegexParser::toPostfix(const std::string& regex) {

    std::string explicitConcat = _addExplicitConcat(regex);
    std::string postfix;
    std::stack<char> operators;

    for (char c: explicitConcat) {
        if (c == '(') {
            operators.push(c);
        } else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            if (!operators.empty()) operators.pop(); // Pop '('
        } else if (c == '*' || c == '+' || c == '?' || c == '.' || c == '|') {
            while (!operators.empty() && operators.top() != '(' && 
                   _getPrecedence(operators.top()) >= _getPrecedence(c)) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        } else {
            // Literal
            postfix += c;
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}

std::string RegexParser::_addExplicitConcat(const std::string& regex) {
    std::string result;
    for (size_t i = 0; i < regex.size(); ++i) {
        char c = regex[i];
        result += c;

        if (i + 1 < regex.size()) {
            char next = regex[i + 1];
            bool isOperator = (c == '(' || c == '|');
            bool nextIsOperator = (next == ')' || next == '*' || next == '+' || next == '?' || next == '|');
            
            if (!isOperator && !nextIsOperator) {
                result += '.';
            }
        }
    }
    return result;
}

int RegexParser::_getPrecedence(char c) {
    if (c == '*' || c == '+' || c == '?') return 3;
    if (c == '.') return 2;
    if (c == '|') return 1;
    return 0;
}
