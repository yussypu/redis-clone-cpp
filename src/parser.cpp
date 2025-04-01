#include "parser.hpp"
#include <sstream>

std::vector<std::string> tokenize(const std::string& input) {
    std::istringstream stream(input);
    std::string token;
    std::vector<std::string> tokens;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}