/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGenerator.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:13:32 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/16 00:15:44 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGENERATOR_HPP
#define CGENERATOR_HPP

#include "AGenerator.hpp"

#include <map>
#include <string>

class CGenerator : public AGenerator {

  public:
    CGenerator() = default;

  protected:
    std::string generateHeader(const LexerParser &parser) override;
    std::string generateTables(const std::vector<DFA> &dfas,
                               const LexerParser &parser,
                               const CompressionConfig &compression) override;
    std::string generateLexerBody(const LexerParser &parser) override;
    std::string generateEofActions(const LexerParser &parser) override;
    std::string generateUserCode(const LexerParser &parser) override;

  private:
    std::string _generateRulesSwitch(const LexerParser &parser);

    static std::vector<int> _computeEC(const std::vector<DFA> &dfas,
                                       const std::map<int, int> &idToIndex,
                                       size_t totalStates);

    static std::vector<int> _computeMetaEC(const std::vector<DFA> &dfas,
                                           const std::map<int, int> &idToIndex,
                                           size_t totalStates,
                                           const std::vector<int> &ec,
                                           int numEC);
};

#endif
