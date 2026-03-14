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

#include "../include/NFA.hpp"
#include "../include/RegexParser.hpp"

#include <stack>
#include <stdexcept>

NFA NFA::fromRegex(const std::vector<Token> &postfix, int &stateCounter) {

    std::stack<NFA> stack;

    for (const auto& token : postfix) {

        if (token.type == CHAR) {
            stack.push(makeChar(token.c, stateCounter));
        } else if (token.type == CHARSET) {
            stack.push(makeSet(token.charSet, stateCounter));
        } else if (token.type == ANCHOR_START) {
            stack.push(makeChar(256, stateCounter));
        } else if (token.type == ANCHOR_END) {
            stack.push(makeChar(257, stateCounter));
        } else if (token.type == TRAILING_CONTEXT_OP) {
            NFA r2 = stack.top(); stack.pop();
            NFA r1 = stack.top(); stack.pop();
            stack.push(makeTrailingContext(r1, r2, stateCounter));
        } else if (token.type == INTERVAL) {
            NFA nfa = stack.top(); stack.pop();
            stack.push(makeRepeat(nfa, token.min, token.max, stateCounter));
        } else if (token.type == OPERATOR) {
            switch (token.c) {
                case RegexParser::CONCAT_OP: {
                    NFA right = stack.top();
                    stack.pop();
                    NFA left = stack.top();
                    stack.pop();
                    stack.push(makeConcat(left, right));
                    break;
                }
                case '.': {
                    stack.push(makeAnyChar(stateCounter));
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
                    // Should not happen for valid postfix
                    throw std::runtime_error("Unknown operator in postfix");
                }
            }
        }
    }

    if (stack.size() != 1) {
        throw std::runtime_error("Invalid regex postfix expression");
    }

    return stack.top();

}

NFA NFA::makeSet(const std::set<int> &chars, int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);
    
    for (int c : chars) {
        start->transitions.insert({c, end});
    }
    return NFA(start, end);
}

NFA NFA::makeChar(int c, int &stateCounter) {
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

NFA NFA::makeAnyChar(int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);
    
    // Add transitions for all characters except newline
    // Assuming 8-bit char -128 to 127
    for (int i = -128; i <= 127; ++i) {
        char c = (char)i;
        if (c != '\n') {
            start->transitions.insert({c, end});
        }
    }
    return NFA(start, end);
}

NFA NFA::makeTrailingContext(NFA r1, NFA r2, int &stateCounter) {
    (void)stateCounter;
    // Mark r1's accept state as the trailing context boundary.
    // This tells the DFA/generator that when this state is reached,
    // it represents the end-of-r1 position to roll back to.
    r1.accept->trailingContextBoundary = true;
    r1.accept->isAccepting = false; // not a final accept — must continue to match r2
    
    // Connect r1's boundary state to r2's start via epsilon
    r1.accept->epsilonTransitions.push_back(r2.start);
    
    // The combined NFA spans r1.start -> ... -> r1.accept (boundary) -> r2 -> r2.accept
    return NFA(r1.start, r2.accept);
}

static std::shared_ptr<State> _copyState(std::shared_ptr<State> s, std::map<int, std::shared_ptr<State>> &clones, int &stateCounter) {
    if (clones.count(s->id)) return clones[s->id];
    auto clone = std::make_shared<State>(stateCounter++);
    clone->isAccepting = s->isAccepting;
    clone->priority = s->priority;
    clone->action = s->action;
    clone->trailingContextBoundary = s->trailingContextBoundary;
    clones[s->id] = clone;

    for (auto const& [c, next] : s->transitions) {
        clone->transitions[c] = _copyState(next, clones, stateCounter);
    }
    for (auto const& next : s->epsilonTransitions) {
        clone->epsilonTransitions.push_back(_copyState(next, clones, stateCounter));
    }
    return clone;
}

NFA NFA::copy(int &stateCounter) const {
    std::map<int, std::shared_ptr<State>> clones;
    auto newStart = _copyState(start, clones, stateCounter);
    auto newAccept = clones[accept->id];
    return NFA(newStart, newAccept);
}

NFA NFA::makeRepeat(NFA nfa, int min, int max, int &stateCounter) {
    if (min == 0 && max == 0) {
        auto s = std::make_shared<State>(stateCounter++);
        auto e = std::make_shared<State>(stateCounter++);
        s->epsilonTransitions.push_back(e);
        return NFA(s, e);
    }

    // Handle min occurrences
    NFA res(nullptr, nullptr);
    if (min > 0) {
        res = nfa.copy(stateCounter);
        for (int i = 1; i < min; ++i) {
            res = makeConcat(res, nfa.copy(stateCounter));
        }
    } else {
        // min == 0, we'll handle below
    }

    if (max == -1) { // {n,}
        NFA kleene = makeKleene(nfa.copy(stateCounter), stateCounter);
        if (min == 0) return kleene;
        return makeConcat(res, kleene);
    } else if (max > min) { // {n,m}
        for (int i = min; i < max; ++i) {
            NFA opt = makeOption(nfa.copy(stateCounter), stateCounter);
            if (res.start == nullptr) res = opt;
            else res = makeConcat(res, opt);
        }
    } else if (min == 0 && max == 0) {
         // Already handled
    }

    if (res.start == nullptr) {
        // This would happen if min=0, max=0 (already handled) 
        // or potentially other weird cases.
        auto s = std::make_shared<State>(stateCounter++);
        auto e = std::make_shared<State>(stateCounter++);
        s->epsilonTransitions.push_back(e);
        return NFA(s, e);
    }

    return res;
}
