/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NFA.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:21:46 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:21:46 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <set>
#include "Token.hpp"

struct State {
    int id;
    bool isAccepting;
    int priority; // -1 for non-accepting, otherwise rule index (lower is better)
    std::string action;
    
    // For trailing context (r1/r2): marks the boundary state where r1 ends.
    // The DFA state that is the boundary gets trailingContextBoundary = true.
    bool trailingContextBoundary;
    
    // Transitions on specific characters (or pseudo-chars > 255)
    std::map<int, std::shared_ptr<State>> transitions;
    
    // Epsilon transitions (\epsilon)
    std::vector<std::shared_ptr<State>> epsilonTransitions;

    State(int id) : id(id), isAccepting(false), priority(-1), trailingContextBoundary(false) {}
};

class NFA {

public:
    std::shared_ptr<State> start;
    std::shared_ptr<State> accept;

    NFA(std::shared_ptr<State> start, std::shared_ptr<State> accept) 
        : start(start), accept(accept) {}

    static NFA fromRegex(const std::vector<Token> &postfix, int &stateCounter);
    
    // Operations
    static NFA makeChar(int c, int &stateCounter);
    static NFA makeSet(const std::set<int> &chars, int &stateCounter);
    static NFA makeConcat(NFA left, NFA right);
    static NFA makeUnion(NFA top, NFA bottom, int &stateCounter);
    static NFA makeKleene(NFA nfa, int &stateCounter);
    static NFA makePlus(NFA nfa, int &stateCounter);
    static NFA makeOption(NFA nfa, int &stateCounter);
    static NFA makeAnyChar(int &stateCounter);
    static NFA makeTrailingContext(NFA r1, NFA r2, int &stateCounter);
    static NFA makeRepeat(NFA nfa, int min, int max, int &stateCounter);

    NFA copy(int &stateCounter) const;

};

#endif
