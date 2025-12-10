/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NFA.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:21:56 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:21:56 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NFA.hpp"
#include <stack>
#include <stdexcept>

NFA NFA::fromRegex(const std::string &postfix, int &stateCounter) {

    std::stack<NFA> stack;

    for (char c: postfix) {

        switch (c) {

            case '.': {
                
                NFA right = stack.top();
                stack.pop();

                
                NFA left = stack.top();
                stack.pop();

                stack.push(makeConcat(left, right));
                break;

            }
            
            case '|': {
                
                NFA bottom = stack.top();
                stack.pop();

                
                NFA top = stack.top();
                stack.pop();

                stack.push(makeUnion(top, bottom, stateCounter));
                break;

            }
            
            case '*': {
                
                NFA nfa = stack.top();
                stack.pop();

                stack.push(makeKleene(nfa, stateCounter));
                break;

            }
            
            case '+': {
                
                NFA nfa = stack.top();
                stack.pop();

                stack.push(makePlus(nfa, stateCounter));
                break;

            }
            
            case '?': {
                
                NFA nfa = stack.top();
                stack.pop();

                stack.push(makeOption(nfa, stateCounter));
                break;

            }
            
            default: {
                stack.push(makeChar(c, stateCounter));
                break;
            }

        }
    
    }

    if (stack.size() != 1) {
        throw std::runtime_error("Invalid regex postfix expression");
    }

    return stack.top();

}

NFA NFA::makeChar(char c, int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);
    start->transitions.insert({c, end});
    return NFA(start, end);
}

NFA NFA::makeConcat(NFA left, NFA right) {
    // left.end -> epsilon -> right.start (Optimization: merge them?)
    // Standard Thompson: Connect end of left to start of right via epsilon logic
    // Actually, Thompson often just merges the states, or adds epsilon. 
    // Epsilon is safer.
    left.accept->epsilonTransitions.push_back(right.start);
    left.accept->isAccepting = false; // It's no longer final
    return NFA(left.start, right.accept);
}

NFA NFA::makeUnion(NFA top, NFA bottom, int& stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    // New start splits to both NFA starts
    start->epsilonTransitions.push_back(top.start);
    start->epsilonTransitions.push_back(bottom.start);

    // Both NFA ends go to new end
    top.accept->epsilonTransitions.push_back(end);
    bottom.accept->epsilonTransitions.push_back(end);

    top.accept->isAccepting = false;
    bottom.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makeKleene(NFA nfa, int& stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    // Start -> NFA start
    start->epsilonTransitions.push_back(nfa.start);
    // Start -> End (0 occurrences)
    start->epsilonTransitions.push_back(end);
    
    // NFA end -> NFA start (Loop)
    nfa.accept->epsilonTransitions.push_back(nfa.start);
    // NFA end -> End
    nfa.accept->epsilonTransitions.push_back(end);

    nfa.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makePlus(NFA nfa, int& stateCounter) {
    // A+ is A A*
    // Construct new NFA that ensures at least one pass
    // Simple way: 
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(nfa.start);
    
    nfa.accept->epsilonTransitions.push_back(nfa.start); // Loop back
    nfa.accept->epsilonTransitions.push_back(end);       // Exit

    nfa.accept->isAccepting = false;
    
    return NFA(start, end);
}

NFA NFA::makeOption(NFA nfa, int& stateCounter) {
    // A? is A|epsilon
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(nfa.start); // Try A
    start->epsilonTransitions.push_back(end);       // Skip A

    nfa.accept->epsilonTransitions.push_back(end);
    nfa.accept->isAccepting = false;

    return NFA(start, end);
}
