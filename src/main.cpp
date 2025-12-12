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
#include <fstream>
#include <string>
#include <memory>

#include "NFA.hpp"
#include "DFA.hpp"
#include "RegexParser.hpp"

#include "AGenerator.hpp"
#include "CGenerator.hpp"
#include "PythonGenerator.hpp"

int main(int argc, char** argv) {

    std::string target_lang = "c";
    
    if (argc > 2) {
        target_lang = argv[2];
    }
    
    std::string output_filename;
    
    if (target_lang == "c") {
        output_filename = "lex.yy.c";
    }
    
    else if (target_lang == "python") {
        output_filename = "lex.yy.py";
    }
    
    else {
        throw std::runtime_error("Unsupported target language '" + target_lang + "'. Use 'c' or 'python'.");
    }

    std::cout << "ft_lex: Processing " << argv[1] << " for target '" << target_lang << "'..." << std::endl;
    
    try {

        LexerParser parser(argv[1]);
        parser.parse();

        int stateCounter = 0;
        auto masterStart = std::make_shared<State>(stateCounter++);
        std::vector<NFA> allNFAs;

        std::cout << "Parsed " << parser.getRulesList().size() << " rules:\n";
        
        int priority = 0;
        for (const auto& rule : parser.getRulesList()) {
            std::vector<Token> postfix = RegexParser::toPostfix(rule.regex);
            
            try {
                NFA nfa = NFA::fromRegex(postfix, stateCounter);
                
                // Configure accepting state
                nfa.accept->isAccepting = true;
                nfa.accept->priority = priority++;
                nfa.accept->action = rule.action;
                
                // Connect master start to nfa start
                masterStart->epsilonTransitions.push_back(nfa.start);
                
                allNFAs.push_back(nfa);
            } catch (const std::exception& e) {
                std::cout << " -> NFA Error: " << e.what() << "\n";
            }
        }
        
        auto dummyAccept = std::make_shared<State>(stateCounter++);
        NFA masterNFA(masterStart, dummyAccept);
        
        std::cout << "Master NFA created (" << stateCounter << " states total).\n";

        int dfaStateCounter = 0;
        DFA dfa = DFA::fromNFA(masterNFA, dfaStateCounter);
        
        std::cout << "DFA Construction Complete: " << dfa.states.size() << " states.\n";
    
        std::unique_ptr<AGenerator> generator;

        if (target_lang == "c") {
            generator = std::make_unique<CGenerator>();
        }
        
        else if (target_lang == "python") {
            generator = std::make_unique<PythonGenerator>();
        }

        else {
            throw std::runtime_error("Unsupported target language '" + target_lang + "'. Use 'c' or 'python'.");
        }

        std::ofstream outfile(output_filename);
        
        if (!outfile.is_open()) {
            throw std::runtime_error("Could not create " + output_filename);
        }
        
        generator->generate(dfa, parser, outfile); 
        
        outfile.close();
        std::cout << "Generated " << output_filename << " successfully.\n";

    }
    
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;

}
