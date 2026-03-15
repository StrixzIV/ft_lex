/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AGenerator.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:11:33 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/16 00:14:49 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AGENERATOR_HPP
#define AGENERATOR_HPP

#include <iosfwd>
#include <string>
#include <vector>

struct CompressionConfig;

class DFA;
class LexerParser;
class CGenerator;
class PythonGenerator;

class AGenerator {

  public:
    void generate(const std::vector<DFA> &dfas, const LexerParser &parser,
                  const CompressionConfig &compression, std::ostream &out);
    virtual ~AGenerator() = default;

  protected:
    virtual std::string generateHeader(const LexerParser &parser) = 0;
    virtual std::string generateTables(const std::vector<DFA> &dfas,
                                       const LexerParser &parser,
                                       const CompressionConfig &compression) = 0;
    virtual std::string generateLexerBody(const LexerParser &parser) = 0;
    virtual std::string generateEofActions(const LexerParser &parser) = 0;
    virtual std::string generateUserCode(const LexerParser &parser) = 0;

    std::string loadTemplate(const std::string &template_key);
};

#endif
