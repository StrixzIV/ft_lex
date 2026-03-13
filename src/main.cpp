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
        int dfaStateCounter = 0;
        std::vector<DFA> dfas;

        const auto& startConds = parser.getStartConditions();
        std::cout << "Parsed " << parser.getRulesList().size() << " rules across " << startConds.size() << " start conditions.\n";

        for (size_t c_idx = 0; c_idx < startConds.size(); ++c_idx) {
            auto masterStart = std::make_shared<State>(stateCounter++);
            
            int priority = 0;
            for (const auto& rule : parser.getRulesList()) {
                
                // Check if this rule applies to the current start condition
                bool applies = false;
                if (rule.startConditions.empty()) {
                    if (!startConds[c_idx].isExclusive) applies = true;
                } else {
                    for (const auto& sc : rule.startConditions) {
                        if (sc == "*" || sc == startConds[c_idx].name) {
                            applies = true;
                            break;
                        }
                    }
                }

                if (!applies) {
                    priority++; // essential: keep rule index aligned with priority
                    continue;
                }

                std::vector<Token> postfix = RegexParser::toPostfix(rule.regex);
                
                try {
                    NFA nfa = NFA::fromRegex(postfix, stateCounter);
                    // Configure accepting state
                    nfa.accept->isAccepting = true;
                    nfa.accept->priority = priority; 
                    nfa.accept->action = rule.action;
                    
                    // Connect master start to nfa start
                    masterStart->epsilonTransitions.push_back(nfa.start);
                } catch (const std::exception& e) {
                    std::cout << " -> NFA Error: " << e.what() << "\n";
                }

                priority++;
            }
            
            auto dummyAccept = std::make_shared<State>(stateCounter++);
            NFA masterNFA(masterStart, dummyAccept);
            DFA dfa = DFA::fromNFA(masterNFA, dfaStateCounter);
            dfas.push_back(dfa);
            
            std::cout << "Start Condition '" << startConds[c_idx].name << "': " << dfa.states.size() << " DFA states.\n";
        }
    
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
        if (!outfile) {
            throw std::runtime_error("Could not open output file: " + output_filename);
        }
        
        generator->generate(dfas, parser, outfile);
        
        outfile.close();
        std::cout << "Generated " << output_filename << " successfully.\n";

    }
    
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;

}
