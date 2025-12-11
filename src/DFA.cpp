/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DFA.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:34:54 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:34:54 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DFA.hpp"
#include <queue>
#include <stack>
#include <algorithm>
#include <iostream>

DFA DFA::fromNFA(const NFA &nfa, int &dfaStateCounter) {
    DFA dfa;
    std::map<std::set<int>, std::shared_ptr<DFAState>> dfaStatesMap;
    std::queue<std::shared_ptr<DFAState>> worklist;

    // 1. Initial Epsilon Closure for Start State
    std::set<std::shared_ptr<State>> startSet;
    startSet.insert(nfa.start);
    startSet = _epsilonClosure(startSet);

    // Create Initial DFA State
    auto startDFA = std::make_shared<DFAState>(dfaStateCounter++);
    std::set<int> startIds;
    for (const auto &s : startSet) {
        startIds.insert(s->id);
    }
    startDFA->nfaStates = startIds;

    // Check acceptance
    int winningPriority = -1;
    for (const auto &s : startSet) {
        if (s->isAccepting) {
            if (winningPriority == -1 || (s->priority != -1 && s->priority < winningPriority)) {
                winningPriority = s->priority;
                startDFA->isAccepting = true;
                startDFA->priority = s->priority;
                startDFA->action = s->action;
            }
        }
    }
    
    dfa.start = startDFA;
    dfa.states.push_back(startDFA);
    dfaStatesMap[startIds] = startDFA;
    worklist.push(startDFA);

    // Build ID->State* map once
    std::map<int, std::shared_ptr<State>> idToState;
    {
         std::set<int> visited;
         std::queue<std::shared_ptr<State>> q;
         
         q.push(nfa.start);
         visited.insert(nfa.start->id);
         idToState[nfa.start->id] = nfa.start;
         
         while (!q.empty()) {
             auto curr = q.front();
             q.pop();
             
             for (auto &t : curr->transitions) {
                 if (visited.find(t.second->id) == visited.end()) {
                     visited.insert(t.second->id);
                     idToState[t.second->id] = t.second;
                     q.push(t.second);
                 }
             }
             
             for (auto &ep : curr->epsilonTransitions) {
                 if (visited.find(ep->id) == visited.end()) {
                     visited.insert(ep->id);
                     idToState[ep->id] = ep;
                     q.push(ep);
                 }
             }
         }
    }

    // 2. Process Worklist
    while (!worklist.empty()) {
        auto currentDFA = worklist.front();
        worklist.pop();

        std::set<char> inputs;
        
        // Reconstruct current Set of State*
        std::set<std::shared_ptr<State>> currentSet;
        for (int id : currentDFA->nfaStates) {
            currentSet.insert(idToState[id]);
        }

        // Collect inputs
        for (const auto &s : currentSet) {
            for (const auto &t : s->transitions) {
                inputs.insert(t.first);
            }
        }

        for (char c : inputs) {
            std::set<std::shared_ptr<State>> moveSet = _move(currentSet, c);
            std::set<std::shared_ptr<State>> closureSet = _epsilonClosure(moveSet);

            if (closureSet.empty()) {
                continue; // Dead state, implicit
            }

            std::set<int> nextIds;
            for (const auto &s : closureSet) {
                nextIds.insert(s->id);
            }

            if (dfaStatesMap.find(nextIds) == dfaStatesMap.end()) {
                auto newDFA = std::make_shared<DFAState>(dfaStateCounter++);
                newDFA->nfaStates = nextIds;
                
                // Acceptance logic
                int winP = -1;
                for (const auto &s : closureSet) {
                    if (s->isAccepting) {
                        if (winP == -1 || (s->priority != -1 && s->priority < winP)) {
                            winP = s->priority;
                            newDFA->isAccepting = true;
                            newDFA->priority = s->priority;
                            newDFA->action = s->action;
                        }
                    }
                }

                dfa.states.push_back(newDFA);
                dfaStatesMap[nextIds] = newDFA;
                worklist.push(newDFA);
                currentDFA->transitions[c] = newDFA;
            } else {
                currentDFA->transitions[c] = dfaStatesMap[nextIds];
            }
        }
    }
    
    return dfa;
}

std::set<std::shared_ptr<State>> DFA::_epsilonClosure(const std::set<std::shared_ptr<State>> &states) {
    std::set<std::shared_ptr<State>> closure = states;
    std::stack<std::shared_ptr<State>> state_stack;
    
    for (const auto &s : states) {
        state_stack.push(s);
    }

    while (!state_stack.empty()) {
        auto s = state_stack.top();
        state_stack.pop();
        
        for (const auto &next : s->epsilonTransitions) {
            if (closure.find(next) == closure.end()) {
                closure.insert(next);
                state_stack.push(next);
            }
        }
    }

    return closure;
}

std::set<std::shared_ptr<State>> DFA::_move(const std::set<std::shared_ptr<State>> &states, char c) {
    std::set<std::shared_ptr<State>> result;
    for (const auto &s : states) {
        auto range = s->transitions.equal_range(c);
        for (auto it = range.first; it != range.second; ++it) {
            result.insert(it->second);
        }
    }
    return result;
}
