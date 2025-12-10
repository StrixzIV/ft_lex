/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:12:59 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/11 02:12:59 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEXPARSER_HPP
#define REGEXPARSER_HPP

# include <string>

class RegexParser {

    public:
        static std::string toPostfix(const std::string &regex);
        
        // Internal char used for concatenation
        static const char CONCAT_OP = 1; // SOH (0x01)

    private:
        static int _getPrecedence(char c);
        static std::string _addExplicitConcat(const std::string &regex);

    };

#endif
