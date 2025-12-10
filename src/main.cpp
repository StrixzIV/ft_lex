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
#include "../include/ft_lex.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./ft_lex [file.l]" << std::endl;
        return 1;
    }
    
    std::cout << "ft_lex: Processing " << argv[1] << "..." << std::endl;
    // TODO: Implement parsing and generation
    
    return 0;
}
