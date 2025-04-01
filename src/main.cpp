#include <iostream>
#include <string>
#include "store.hpp"
#include "parser.hpp"

int main() {
    Store store;

    std::cout << "Welcome to my Redis Clone\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string command = tokens[0];

        if (command == "EXIT" || command == "QUIT") {
            break;
        }

        // SET key value (support multi-word value)
        else if (command == "SET" && tokens.size() >= 3) {
            std::string key = tokens[1];
            std::string value;
            for (size_t i = 2; i < tokens.size(); ++i) {
                value += tokens[i];
                if (i != tokens.size() - 1) value += " ";
            }
            store.set(key, value);
            std::cout << "OK\n";
        }

        // GET key
        else if (command == "GET" && tokens.size() == 2) {
            auto result = store.get(tokens[1]);
            if (result) {
                std::cout << *result << "\n";
            } else {
                std::cout << "(nil)\n";
            }
        }

        // Unknown command
        else {
            std::cout << "Unknown or invalid command.\n";
        }
    }

    return 0;
}
