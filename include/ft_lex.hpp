/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lex.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 01:30:55 by jikaewsi          #+#    #+#             */
/*   Updated: 2026/03/16 00:14:27 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LEX_HPP
#define FT_LEX_HPP

#include <memory>
#include <string>
#include <vector>

#include "LexerParser.hpp"

/**
 * Compression configuration for generated DFA tables.
 *
 *  -f / -Cf   Full tables  (no compression, yy_nxt[N][258])
 *  -Ce        Equivalence classes  (yy_ec[258] + yy_nxt[N][numEC])
 *  -Cm        Meta-equivalence classes (implies -Ce; adds yy_meta[numEC])
 *  -F / -CF   Fast tables  (equivalent to -Cem)
 */
struct CompressionConfig {
    bool useEC     = false; // -Ce
    bool useMetaEC = false; // -Cm (implies useEC)
    // When both are false → full tables (-f / -Cf, the default)
};

#endif
