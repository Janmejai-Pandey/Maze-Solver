#pragma once
#include <string>
#include <iostream>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BOLD    = "\033[1m";

    // Bright / Light variants
    const std::string LIGHT_BLUE   = "\033[94m";    // light blue text
    const std::string LIGHT_CYAN   = "\033[96m";    // light cyan text

    // Background colors
    const std::string BG_GREEN     = "\033[42m";
    const std::string BG_BLUE      = "\033[44m";
    const std::string BG_YELLOW    = "\033[43m";
    const std::string BG_RED       = "\033[41m";
    const std::string BG_WHITE     = "\033[47m";
    const std::string BG_CYAN      = "\033[46m";
    const std::string BG_LIGHT_BLUE = "\033[104m";  // light blue background
}

namespace Utils {

inline void printBanner() {
    std::cout << Color::CYAN << Color::BOLD;
    std::cout << "+==========================================+\n";
    std::cout << "|       C++ MAZE SOLVER PROJECT            |\n";
    std::cout << "|    BFS vs DFS Algorithm Showcase         |\n";
    std::cout << "+==========================================+\n";
    std::cout << Color::RESET << "\n";
}

inline void printSeparator(const std::string& title = "") {
    std::cout << Color::YELLOW;
    if (title.empty()) {
        std::cout << "------------------------------------------\n";
    } else {
        std::cout << "---- " << title << " ";
        int padding = 36 - (int)title.size();
        if (padding < 0) padding = 0;
        for (int i = 0; i < padding; i++) std::cout << "-";
        std::cout << "\n";
    }
    std::cout << Color::RESET;
}

} // namespace Utils