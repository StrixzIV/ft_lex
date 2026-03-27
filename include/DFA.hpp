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

struct State;
class NFA;
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct DFAState {
    int id;
    std::set<int> nfaStates;

    bool isAccepting;
    int priority;
    std::vector<int> acceptingRules;
    std::string action;
    bool trailingContextBoundary;

    std::map<int, std::shared_ptr<DFAState>> transitions;

    DFAState(int id)
        : id(id), isAccepting(false), priority(-1),
          trailingContextBoundary(false) {}
};

class DFA {

  public:
    std::shared_ptr<DFAState> start;
    std::vector<std::shared_ptr<DFAState>> states;

    static DFA fromNFA(const NFA &nfa, int &dfaStateCounter);

  private:
    static std::set<std::shared_ptr<State>>
    _epsilonClosure(const std::set<std::shared_ptr<State>> &states);
    static std::set<std::shared_ptr<State>>
    _move(const std::set<std::shared_ptr<State>> &states, int c);
};

#endif
