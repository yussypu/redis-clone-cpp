#include <iostream>
#include <string>
#include "store.hpp"
#include "parser.hpp"

// ANSI color codes
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

int main() {
    Store store;

    store.loadFromFile("dump.rdb");

    std::cout << CYAN;
    std::cout << R"(

██╗  ██╗██╗██████╗ ███████╗    ███╗   ███╗███████╗
██║  ██║██║██╔══██╗██╔════╝    ████╗ ████║██╔════╝
███████║██║██████╔╝█████╗      ██╔████╔██║█████╗  
██╔══██║██║██╔══██╗██╔══╝      ██║╚██╔╝██║██╔══╝  
██║  ██║██║██║  ██║███████╗    ██║ ╚═╝ ██║███████╗
╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝    ╚═╝     ╚═╝╚══════╝
                                                  
Redis Clone by Yahya 🧠 - Type HELP for commands

)" << RESET;

    std::string line;
    while (true) {
        std::cout << RED << "redis-clone> " << RESET;
        std::getline(std::cin, line);

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string command = tokens[0];

        if (command == "EXIT" || command == "QUIT") {
            break;
        }

        else if (command == "SET" && tokens.size() >= 3) {
            std::string key = tokens[1];
            std::string value;
            for (size_t i = 2; i < tokens.size(); ++i) {
                value += tokens[i];
                if (i != tokens.size() - 1) value += " ";
            }
            store.set(key, value);
            std::cout << GREEN << "✅ OK\n" << RESET;
        }

        else if (command == "GET" && tokens.size() == 2) {
            auto result = store.get(tokens[1]);
            if (result) {
                std::cout << *result << "\n";
            } else {
                std::cout << "(nil)\n";
            }
        }

        else if (command == "DEL" && tokens.size() == 2) {
            bool removed = store.del(tokens[1]);
            std::cout << (removed ? "(1)\n" : "(0)\n");
        }

        else if (command == "EXISTS" && tokens.size() == 2) {
            std::cout << (store.exists(tokens[1]) ? "(1)\n" : "(0)\n");
        }

        else if (command == "KEYS") {
            auto all = store.keys();
            if (all.empty()) {
                std::cout << "(empty)\n";
            } else {
                for (const auto& k : all) {
                    std::cout << k << "\n";
                }
            }
        }

        else if (command == "FLUSHALL") {
            store.flush();
            std::cout << GREEN << "✅ OK\n" << RESET;
        }

        else if (command == "EXPIRE" && tokens.size() == 3) {
            try {
                int seconds = std::stoi(tokens[2]);
                bool ok = store.expire(tokens[1], seconds);
                std::cout << (ok ? "(1)\n" : "(0)\n");
            } catch (...) {
                std::cout << RED << "❌ Invalid number of seconds.\n" << RESET;
            }
        }

        else if (command == "TTL" && tokens.size() == 2) {
            int result = store.ttl(tokens[1]);
            if (result == -2) {
                std::cout << "(key not found or expired)\n";
            } else if (result == -1) {
                std::cout << "(no expiration)\n";
            } else {
                std::cout << result << " seconds\n";
            }
        }

        else if (command == "SAVE") {
            bool ok = store.saveToFile("dump.rdb");
            std::cout << (ok ? GREEN "✅ Saved to dump.rdb\n" RESET : RED "❌ Save failed\n" RESET);
        }

        else if (command == "LOAD") {
            bool ok = store.loadFromFile("dump.rdb");
            std::cout << (ok ? GREEN "✅ Loaded from dump.rdb\n" RESET : RED "❌ Load failed\n" RESET);
        }

        else if (command == "CLEAR") {
            std::cout << "\033[2J\033[1;1H"; // Clear screen
        }

        else if (command == "HELP") {
            std::cout << YELLOW
                      << "Supported commands:\n"
                      << "  SET key value       - Store a value\n"
                      << "  GET key             - Retrieve a value\n"
                      << "  DEL key             - Delete a key\n"
                      << "  EXISTS key          - Check if key exists\n"
                      << "  KEYS                - List all keys\n"
                      << "  FLUSHALL            - Clear all data\n"
                      << "  EXPIRE key seconds  - Set a TTL for a key\n"
                      << "  TTL key             - Show time until expiration\n"
                      << "  SAVE                - Save to disk\n"
                      << "  LOAD                - Load from disk\n"
                      << "  CLEAR               - Clear the screen\n"
                      << "  EXIT or QUIT        - Exit the program\n"
                      << RESET;
        }

        else {
            std::cout << RED << "❌ Unknown or invalid command. Type HELP for options.\n" << RESET;
        }
    }

    store.saveToFile("dump.rdb");

    return 0;
}
