/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AGenerator.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:23:08 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 03:19:22 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "template.hpp"
#include "AGenerator.hpp"
#include "CGenerator.hpp"
#include "PythonGenerator.hpp"

#include <string_view>

std::string AGenerator::loadTemplate(const std::string& template_key) {

    std::size_t size = 0;
    const unsigned char* start = nullptr;

    if (template_key == "C") {
        size = TEMPLATE_C_SIZE;
        start = TEMPLATE_C_START;
    }
    
    else if (template_key == "Python") {
        size = TEMPLATE_PY_SIZE;
        start = TEMPLATE_PY_START;
    }
    
    else {
        throw std::runtime_error("Unknown template key provided to AGenerator::loadTemplate");
    }

    if (!start || size == 0) {
        throw std::runtime_error("Template data is missing or empty for key: " + template_key);
    }

    std::string_view template_view(
        reinterpret_cast<const char*>(start),
        size
    );
    
    return std::string(template_view);

}

void AGenerator::generate(const DFA &dfa, const LexerParser &parser, std::ostream &out) {

    std::string template_key = "Unknown";

    if (typeid(*this) == typeid(CGenerator)) {
        template_key = "C";
    }
    
    else if (typeid(*this) == typeid(PythonGenerator)) {
        template_key = "Python";
    }

    std::string output_code = loadTemplate(template_key);

    std::string header = generateHeader(parser);
    std::string tables = generateTables(dfa, parser);
    std::string lexer_body = generateLexerBody(parser);
    std::string user_code = generateUserCode(parser);

    replace_placeholder(output_code, "__HEADER_PLACEHOLDER__", header);
    replace_placeholder(output_code, "__TABLES_PLACEHOLDER__", tables);
    replace_placeholder(output_code, "__YYLEX_BODY_PLACEHOLDER__", lexer_body);
    replace_placeholder(output_code, "__USER_CODE_PLACEHOLDER__", user_code);

    out << output_code;

}
