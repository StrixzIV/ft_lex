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
#include <vector>
#include <unistd.h>

#include "LexerParser.hpp"
#include "NFA.hpp"
#include "DFA.hpp"
#include "RegexParser.hpp"

#include "AGenerator.hpp"
#include "CGenerator.hpp"
#include "PythonGenerator.hpp"

void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [-t] [-o file] [-l lang] <lexer.l>...\n";
    std::cerr << "  -t: Write generated code to stdout\n";
    std::cerr << "  -o file: Write generated code to 'file' (default: lex.yy.c or lex.yy.py)\n";
    std::cerr << "  -l lang: Target language 'c' or 'python' (default: c)\n";
}

int main(int argc, char** argv) {

    bool to_stdout = false;
    std::string target_lang = "c";
    std::string output_filename = "";
    std::vector<std::string> input_filenames;

    int opt;
    while ((opt = getopt(argc, argv, "to:l:")) != -1) {
        switch (opt) {
            case 't':
                to_stdout = true;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'l':
                target_lang = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        print_usage(argv[0]);
        return 1;
    }

    for (int i = optind; i < argc; ++i) {
        input_filenames.push_back(argv[i]);
    }

    if (target_lang != "c" && target_lang != "python") {
        std::cerr << "Error: Unsupported target language '" << target_lang << "'. Use 'c' or 'python'.\n";
        return 1;
    }

    if (output_filename.empty() && !to_stdout) {
        output_filename = (target_lang == "c") ? "lex.yy.c" : "lex.yy.py";
    }

    try {

        LexerParser parser(input_filenames);
        parser.parse();

        int stateCounter = 0;
        int dfaStateCounter = 0;
        std::vector<DFA> dfas;

        const auto& startConds = parser.getStartConditions();
        const auto& rules = parser.getRulesList();

        std::cout << "ft_lex: Processing " << input_filenames.size() << " files for target '" << target_lang << "'...\n";
        std::cout << "ft_lex: Parsed " << rules.size() << (rules.size() == 1 ? " rule" : " rules") 
                  << " across " << startConds.size() << " start conditions.\n";

        for (size_t c_idx = 0; c_idx < startConds.size(); ++c_idx) {
            auto masterStart = std::make_shared<State>(stateCounter++);
            auto bolStart = std::make_shared<State>(stateCounter++);
            masterStart->transitions.insert({256, bolStart});

            int priority = 0;
            for (const auto& rule : rules) {
                
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

                try {
                    std::vector<Token> postfix = RegexParser::toPostfix(rule.regex);
                    
                    // Check if the rule has a BOL anchor at the very beginning
                    // Postfix for ^abc is: ^, a, CONCAT, b, CONCAT, c, CONCAT
                    // Actually ^ is the first token in prefix, but in postfix it depends.
                    // Let's check the original regex for simplicity, or look at tokens.
                    bool hasBOL = (rule.regex.size() > 0 && rule.regex[0] == '^');

                    NFA nfa = NFA::fromRegex(postfix, stateCounter);
                    // Configure accepting state
                    nfa.accept->isAccepting = true;
                    nfa.accept->priority = priority; 
                    nfa.accept->action = rule.action;
                    
                    if (hasBOL) {
                        // For rules with ^, they MUST start after the BOL pseudo-char 256.
                        // But NFA::fromRegex already included the 256 transition if it saw ^.
                        // Wait! If NFA::fromRegex includes 256, then nfa.start IS the state
                        // that expects 256.
                        // So we connect masterStart to nfa.start.
                        masterStart->epsilonTransitions.push_back(nfa.start);
                    } else {
                        // For rules WITHOUT ^, they can start EITHER from masterStart
                        // OR from bolStart (if we are at the beginning of a line).
                        masterStart->epsilonTransitions.push_back(nfa.start);
                        bolStart->epsilonTransitions.push_back(nfa.start);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Lexer:" << rule.lineNo << ": error: " << e.what() << "\n";
                    return 1;
                }

                priority++;
            }
            
            auto dummyAccept = std::make_shared<State>(stateCounter++);
            NFA masterNFA(masterStart, dummyAccept);
            DFA dfa = DFA::fromNFA(masterNFA, dfaStateCounter);
            dfas.push_back(dfa);
        }
    
        std::unique_ptr<AGenerator> generator;

        if (target_lang == "c") {
            generator = std::make_unique<CGenerator>();
        } else {
            generator = std::make_unique<PythonGenerator>();
        }
        
        if (to_stdout) {
            generator->generate(dfas, parser, std::cout);
        } else {
            std::ofstream outfile(output_filename);
            if (!outfile) {
                throw std::runtime_error("Could not open output file: " + output_filename);
            }
            generator->generate(dfas, parser, outfile);
            outfile.close();
            std::cout << "Generated " << output_filename << " successfully.\n";
        }

    }
    
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;

}
