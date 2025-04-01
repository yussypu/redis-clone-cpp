#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

class Store {
public:
    Store();                
    ~Store();               // stops thread

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    std::vector<std::string> keys();
    void flush();
    bool expire(const std::string& key, int seconds); 
    int ttl(const std::string& key); 
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);


private:
    std::unordered_map<std::string, std::string> data;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expiry;

    std::mutex mtx;
    bool running;
    std::thread cleanup_thread;

    void startCleanupThread();
    void cleanupExpiredKeys();
};
