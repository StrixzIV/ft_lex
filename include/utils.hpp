/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jikaewsi <strixz.self@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 01:18:50 by jikaewsi          #+#    #+#             */
/*   Updated: 2025/12/13 01:19:16 by jikaewsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

# include <string>
# include <string_view>

std::string replace_placeholder(std::string &source, const std::string_view &placeholder, const std::string &replacement);

#endif