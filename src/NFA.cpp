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

    for (const auto &token : postfix) {

        if (token.type == CHAR) {
            stack.push(makeChar(token.c, stateCounter));
        } else if (token.type == CHARSET) {
            stack.push(makeSet(token.charSet, stateCounter));
        } else if (token.type == ANCHOR_START) {
            stack.push(makeChar(256, stateCounter));
        } else if (token.type == ANCHOR_END) {
            stack.push(makeChar(257, stateCounter));
        } else if (token.type == TRAILING_CONTEXT_OP) {
            NFA r2 = stack.top();
            stack.pop();
            NFA r1 = stack.top();
            stack.pop();
            stack.push(makeTrailingContext(r1, r2, stateCounter));
        } else if (token.type == INTERVAL) {
            NFA nfa = stack.top();
            stack.pop();
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

    left.accept->epsilonTransitions.push_back(right.start);
    left.accept->isAccepting = false;
    return NFA(left.start, right.accept);
}

NFA NFA::makeUnion(NFA top, NFA bottom, int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(top.start);
    start->epsilonTransitions.push_back(bottom.start);

    top.accept->epsilonTransitions.push_back(end);
    bottom.accept->epsilonTransitions.push_back(end);

    top.accept->isAccepting = false;
    bottom.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makeKleene(NFA nfa, int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(nfa.start);

    start->epsilonTransitions.push_back(end);

    nfa.accept->epsilonTransitions.push_back(nfa.start);

    nfa.accept->epsilonTransitions.push_back(end);

    nfa.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makePlus(NFA nfa, int &stateCounter) {

    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(nfa.start);

    nfa.accept->epsilonTransitions.push_back(nfa.start);
    nfa.accept->epsilonTransitions.push_back(end);

    nfa.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makeOption(NFA nfa, int &stateCounter) {

    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

    start->epsilonTransitions.push_back(nfa.start);
    start->epsilonTransitions.push_back(end);

    nfa.accept->epsilonTransitions.push_back(end);
    nfa.accept->isAccepting = false;

    return NFA(start, end);
}

NFA NFA::makeAnyChar(int &stateCounter) {
    auto start = std::make_shared<State>(stateCounter++);
    auto end = std::make_shared<State>(stateCounter++);

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

    r1.accept->trailingContextBoundary = true;
    r1.accept->isAccepting = false;

    r1.accept->epsilonTransitions.push_back(r2.start);

    return NFA(r1.start, r2.accept);
}

static std::shared_ptr<State>
_copyState(std::shared_ptr<State> s,
           std::map<int, std::shared_ptr<State>> &clones, int &stateCounter) {
    if (clones.count(s->id))
        return clones[s->id];
    auto clone = std::make_shared<State>(stateCounter++);
    clone->isAccepting = s->isAccepting;
    clone->priority = s->priority;
    clone->action = s->action;
    clone->trailingContextBoundary = s->trailingContextBoundary;
    clones[s->id] = clone;

    for (auto const &[c, next] : s->transitions) {
        clone->transitions[c] = _copyState(next, clones, stateCounter);
    }
    for (auto const &next : s->epsilonTransitions) {
        clone->epsilonTransitions.push_back(
            _copyState(next, clones, stateCounter));
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

    NFA res(nullptr, nullptr);
    if (min > 0) {
        res = nfa.copy(stateCounter);
        for (int i = 1; i < min; ++i) {
            res = makeConcat(res, nfa.copy(stateCounter));
        }
    } else {
    }

    if (max == -1) {
        NFA kleene = makeKleene(nfa.copy(stateCounter), stateCounter);
        if (min == 0)
            return kleene;
        return makeConcat(res, kleene);
    } else if (max > min) {
        for (int i = min; i < max; ++i) {
            NFA opt = makeOption(nfa.copy(stateCounter), stateCounter);
            if (res.start == nullptr)
                res = opt;
            else
                res = makeConcat(res, opt);
        }
    } else if (min == 0 && max == 0) {
    }

    if (res.start == nullptr) {

        auto s = std::make_shared<State>(stateCounter++);
        auto e = std::make_shared<State>(stateCounter++);
        s->epsilonTransitions.push_back(e);
        return NFA(s, e);
    }

    return res;
}
