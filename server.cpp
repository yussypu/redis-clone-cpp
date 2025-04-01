#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include "store.hpp"
#include "parser.hpp"

// Handle client requests
void clientHandler(int clientSocket, Store& store);

// Run the server
void runServer(Store& store) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(6379);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed\n";
        return;
    }

    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    std::cout << "Server listening on port 6379\n";

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Connection failed\n";
            continue;
        }
        std::cout << "Client connected\n";
        std::thread(clientHandler, clientSocket, std::ref(store)).detach();
    }

    close(serverSocket);
}

void clientHandler(int clientSocket, Store& store) {
    std::string line;
    while (true) {
        char buffer[1024] = {0};
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cerr << "Client disconnected or error reading data." << std::endl;
            break;
        }

        buffer[bytesRead] = '\0';
        line = buffer;

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        if (line == "EXIT" || line == "QUIT") {
            std::cout << "Closing connection with client..." << std::endl;
            break;
        }

        auto tokens = tokenize(line);
        std::string command = tokens[0];

        if (command == "SET" && tokens.size() >= 3) {
            std::string key = tokens[1];
            std::string value;
            for (size_t i = 2; i < tokens.size(); ++i) {
                value += tokens[i];
                if (i != tokens.size() - 1) value += " ";
            }
            store.set(key, value);
            std::string response = "OK\n";
            write(clientSocket, response.c_str(), response.size());
        } else if (command == "GET" && tokens.size() == 2) {
            auto result = store.get(tokens[1]);
            if (result) {
                std::string response = *result + "\n";
                write(clientSocket, response.c_str(), response.size());
            } else {
                std::string response = "(nil)\n";
                write(clientSocket, response.c_str(), response.size());
            }
        } else if (command == "DEL" && tokens.size() == 2) {
            bool removed = store.del(tokens[1]);
            std::string response = removed ? "(1)\n" : "(0)\n";
            write(clientSocket, response.c_str(), response.size());
        } else if (command == "EXISTS" && tokens.size() == 2) {
            bool exists = store.exists(tokens[1]);
            std::string response = exists ? "(1)\n" : "(0)\n";
            write(clientSocket, response.c_str(), response.size());
        } else if (command == "KEYS") {
            auto all = store.keys();
            if (all.empty()) {
                std::string response = "(empty)\n";
                write(clientSocket, response.c_str(), response.size());
            } else {
                for (const auto& k : all) {
                    std::string response = k + "\n";
                    write(clientSocket, response.c_str(), response.size());
                }
            }
        } else if (command == "FLUSHALL") {
            store.flush();
            std::string response = "OK\n";
            write(clientSocket, response.c_str(), response.size());
        } else if (command == "EXPIRE" && tokens.size() == 3) {
            try {
                int seconds = std::stoi(tokens[2]);
                bool ok = store.expire(tokens[1], seconds);
                std::string response = ok ? "(1)\n" : "(0)\n";
                write(clientSocket, response.c_str(), response.size());
            } catch (...) {
                std::string response = "(invalid seconds)\n";
                write(clientSocket, response.c_str(), response.size());
            }
        } else if (command == "TTL" && tokens.size() == 2) {
            int ttl = store.ttl(tokens[1]);
            std::string response;
            if (ttl == -2) {
                response = "(key not found or expired)\n";
            } else if (ttl == -1) {
                response = "(no expiration)\n";
            } else {
                response = std::to_string(ttl) + " seconds\n";
            }
            write(clientSocket, response.c_str(), response.size());
        } else if (command == "HELP") {
            std::string response = 
                "Supported commands:\n"
                "  SET key value       - Store a value\n"
                "  GET key             - Retrieve a value\n"
                "  DEL key             - Delete a key\n"
                "  EXISTS key          - Check if key exists\n"
                "  KEYS                - List all keys\n"
                "  FLUSHALL            - Clear all data\n"
                "  EXPIRE key seconds  - Set a TTL for a key\n"
                "  TTL key             - Show time until expiration\n"
                "  EXIT / QUIT         - Close the connection\n"
                "  HELP                - Show this message\n";
            write(clientSocket, response.c_str(), response.size());
        }
        
        else {
            std::string response = "(unknown command)\n";
            write(clientSocket, response.c_str(), response.size());
        }
    }

    close(clientSocket);
}