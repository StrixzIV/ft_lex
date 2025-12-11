/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 01:31:08 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 01:31:08 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../include/ft_lex.hpp"
#include "RegexParser.hpp"
#include "NFA.hpp"
#include "DFA.hpp"
#include "Generator.hpp"
#include <fstream>

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: ./ft_lex [file.l]" << std::endl;
        return 1;
    }
    
    std::cout << "ft_lex: Processing " << argv[1] << "..." << std::endl;
    
    try {
        LexerParser parser(argv[1]);
        parser.parse();

        // Temporary debug output
        std::cout << "--- Definitions ---\n" << parser.getDefinitions().size() << " bytes\n";
        std::cout << "--- Rules ---\n" << parser.getRules().size() << " bytes\n";
        
        const auto& rules = parser.getRulesList();

        // ... inside main ...
        int stateCounter = 0;
        
        // Master NFA parts
        auto masterStart = std::make_shared<State>(stateCounter++);
        std::vector<NFA> allNFAs;

        std::cout << "Parsed " << rules.size() << " rules:\n";
        
        int priority = 0;
        for (const auto& rule : rules) {
            std::vector<Token> postfix = RegexParser::toPostfix(rule.regex);
            // std::cout << "  Regex: " << rule.regex << " -> Postfix size: " << postfix.size();
            
            try {
                NFA nfa = NFA::fromRegex(postfix, stateCounter);
                
                // Configure accepting state
                nfa.accept->isAccepting = true;
                nfa.accept->priority = priority++;
                nfa.accept->action = rule.action;
                
                // Connect master start to nfa start
                masterStart->epsilonTransitions.push_back(nfa.start);
                
                allNFAs.push_back(nfa); // Keep them alive? Shared ptrs should handle it if referenced by masterStart
                
                // std::cout << " -> NFA fragment added.\n";
            } catch (const std::exception& e) {
                std::cout << " -> NFA Error: " << e.what() << "\n";
            }
        }
        
        // Technically, the Master NFA doesn't have a single accepting state, but a set of them.
        // But for DFA construction we only need the start state.
        // We create a dummy NFA object to pass valid start state.
        auto dummyAccept = std::make_shared<State>(stateCounter++); // Unused
        NFA masterNFA(masterStart, dummyAccept);
        
        std::cout << "Master NFA created (" << stateCounter << " states total).\n";
        
        // Build DFA
        int dfaStateCounter = 0;
        DFA dfa = DFA::fromNFA(masterNFA, dfaStateCounter);
        
        std::cout << "DFA Construction Complete: " << dfa.states.size() << " states.\n";
        
        // Generate Output
        std::ofstream outfile("lex.yy.c");
        if (!outfile.is_open()) {
            throw std::runtime_error("Could not create lex.yy.c");
        }
        
        Generator::generate(dfa, parser, outfile);
        outfile.close();
        std::cout << "Generated lex.yy.c successfully.\n";

    }
    
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;

}
