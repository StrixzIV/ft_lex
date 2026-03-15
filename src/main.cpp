/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 01:31:08 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/16 00:18:28 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

#include "../include/DFA.hpp"
#include "../include/LexerParser.hpp"
#include "../include/NFA.hpp"
#include "../include/RegexParser.hpp"
#include "../include/ft_lex.hpp"

#include "../include/AGenerator.hpp"
#include "../include/CGenerator.hpp"
#include "../include/PythonGenerator.hpp"

void print_usage(const char *prog) {
    std::cerr << "Usage: " << prog
              << " [-vntc] [-o file] [-l lang] [-C mode] [-f] [-F] <lexer.l>...\n";
    std::cerr << "  -v: Write a summary of scanner statistics to stderr\n";
    std::cerr << "  -n: No-op for POSIX compliance (suppress summary)\n";
    std::cerr << "  -c: No-op for POSIX compliance (C output is default)\n";
    std::cerr << "  -t: Write generated code to stdout\n";
    std::cerr << "  -o file: Write generated code to 'file' (default: lex.yy.c "
                 "or lex.yy.py)\n";
    std::cerr << "  -l lang: Target language 'c' or 'python' (default: c)\n";
    std::cerr << "  -C mode: Compression mode:\n";
    std::cerr << "           e  - Construct equivalence classes\n";
    std::cerr << "           m  - Construct meta-equivalence classes "
                 "(implies -Ce)\n";
    std::cerr << "           f  - Full tables (no compression, default)\n";
    std::cerr << "           F  - Fast tables (equivalent to -Cem)\n";
    std::cerr << "  -f: Same as -Cf (full tables)\n";
    std::cerr << "  -F: Same as -CF (fast tables)\n";
}

int main(int argc, char **argv) {

    bool to_stdout = false;
    bool verbose = false;
    bool suppress_summary = false;
    std::string target_lang = "c";
    std::string output_filename = "";
    std::vector<std::string> input_filenames;
    CompressionConfig compression;

    int opt;
    while ((opt = getopt(argc, argv, "vntcfFo:l:C:")) != -1) {
        switch (opt) {
        case 'v':
            verbose = true;
            break;
        case 'n':
            suppress_summary = true;
            break;
        case 'c':
            target_lang = "c";
            break;
        case 't':
            to_stdout = true;
            break;
        case 'o':
            output_filename = optarg;
            break;
        case 'l':
            target_lang = optarg;
            break;
        case 'f':
            compression.useEC = false;
            compression.useMetaEC = false;
            break;
        case 'F':
            compression.useEC = true;
            compression.useMetaEC = true;
            break;
        case 'C': {
            std::string carg = optarg;
            for (char ch : carg) {
                switch (ch) {
                case 'e':
                    compression.useEC = true;
                    break;
                case 'm':
                    compression.useEC = true;
                    compression.useMetaEC = true;
                    break;
                case 'f':
                    compression.useEC = false;
                    compression.useMetaEC = false;
                    break;
                case 'F':
                    compression.useEC = true;
                    compression.useMetaEC = true;
                    break;
                default:
                    std::cerr << "Error: Unknown compression option '" << ch
                              << "'\n";
                    print_usage(argv[0]);
                    return 1;
                }
            }
            break;
        }
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
        std::cerr << "Error: Unsupported target language '" << target_lang
                  << "'. Use 'c' or 'python'.\n";
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

        const auto &startConds = parser.getStartConditions();
        const auto &rules = parser.getRulesList();

        if (!suppress_summary) {
            std::cout << "ft_lex: Processing " << input_filenames.size()
                      << " files for target '" << target_lang << "'...\n";
            std::cout << "ft_lex: Parsed " << rules.size()
                      << (rules.size() == 1 ? " rule" : " rules") << " across "
                      << startConds.size() << " start conditions.\n";
        }

        size_t total_dfa_states = 0;
        size_t total_dfa_transitions = 0;

        for (size_t c_idx = 0; c_idx < startConds.size(); ++c_idx) {
            auto masterStart = std::make_shared<State>(stateCounter++);
            auto bolStart = std::make_shared<State>(stateCounter++);
            masterStart->transitions.insert({256, bolStart});

            int priority = 0;
            for (const auto &rule : rules) {

                bool applies = false;
                if (rule.startConditions.empty()) {
                    if (!startConds[c_idx].isExclusive)
                        applies = true;
                } else {
                    for (const auto &sc : rule.startConditions) {
                        if (sc == "*" || sc == startConds[c_idx].name) {
                            applies = true;
                            break;
                        }
                    }
                }

                if (!applies) {
                    priority++;
                    continue;
                }

                try {
                    std::vector<Token> postfix =
                        RegexParser::toPostfix(rule.regex);

                    bool hasBOL =
                        (rule.regex.size() > 0 && rule.regex[0] == '^');

                    NFA nfa = NFA::fromRegex(postfix, stateCounter);

                    nfa.accept->isAccepting = true;
                    nfa.accept->priority = priority;
                    nfa.accept->action = rule.action;

                    if (hasBOL) {
                        masterStart->epsilonTransitions.push_back(nfa.start);
                    } else {
                        masterStart->epsilonTransitions.push_back(nfa.start);
                        bolStart->epsilonTransitions.push_back(nfa.start);
                    }
                } catch (const std::exception &e) {
                    std::cerr << parser.formatError(rule.lineNo, 1, e.what())
                              << "\n";
                    return 1;
                }

                priority++;
            }

            auto dummyAccept = std::make_shared<State>(stateCounter++);
            NFA masterNFA(masterStart, dummyAccept);
            DFA dfa = DFA::fromNFA(masterNFA, dfaStateCounter);

            total_dfa_states += dfa.states.size();
            for (const auto &s : dfa.states) {
                total_dfa_transitions += s->transitions.size();
            }

            dfas.push_back(dfa);
        }

        if (verbose) {
            std::cerr << "DFA Statistics:\n";
            std::cerr << "  Total NFA states: " << stateCounter << "\n";
            std::cerr << "  Total DFA states: " << total_dfa_states << "\n";
            std::cerr << "  Total transitions: " << total_dfa_transitions
                      << "\n";
            if (compression.useEC || compression.useMetaEC) {
                std::cerr << "  Compression: ";
                if (compression.useMetaEC)
                    std::cerr << "meta-equivalence classes (-Cm)\n";
                else
                    std::cerr << "equivalence classes (-Ce)\n";
                std::cerr << "  Full table entries: "
                          << total_dfa_states * 258 << " ints\n";
            } else {
                std::cerr << "  Compression: none (full tables, -Cf)\n";
                std::cerr << "  Table entries: "
                          << total_dfa_states * 258 << " ints\n";
            }
        }

        std::unique_ptr<AGenerator> generator;

        if (target_lang == "c") {
            generator = std::make_unique<CGenerator>();
        } else {
            generator = std::make_unique<PythonGenerator>();
        }

        if (to_stdout) {
            generator->generate(dfas, parser, compression, std::cout);
        } else {
            std::ofstream outfile(output_filename);
            if (!outfile) {
                throw std::runtime_error("Could not open output file: " +
                                         output_filename);
            }
            generator->generate(dfas, parser, compression, outfile);
            outfile.close();
            std::cout << "Generated " << output_filename << " successfully.\n";
        }

    }

    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
