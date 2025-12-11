/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DFA.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:34:42 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:34:42 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DFA_HPP
#define DFA_HPP

#include "NFA.hpp"
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>

struct DFAState {
    int id;
    std::set<int> nfaStates; // IDs of NFA states
    
    bool isAccepting;
    int priority; // Winning priority
    std::string action;

    std::map<char, std::shared_ptr<DFAState>> transitions;

    DFAState(int id) : id(id), isAccepting(false), priority(-1) {}
};

class DFA {

    public:
        std::shared_ptr<DFAState> start;
        std::vector<std::shared_ptr<DFAState>> states;

        static DFA fromNFA(const NFA &nfa, int &dfaStateCounter);

    private:
        static std::set<std::shared_ptr<State>> _epsilonClosure(const std::set<std::shared_ptr<State>> &states);
        static std::set<std::shared_ptr<State>> _move(const std::set<std::shared_ptr<State>> &states, char c);

};

#endif
