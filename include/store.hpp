#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <fstream>
#include <json/json.hpp>  // For JSON handling

class Store {
private:
    std::unordered_map<std::string, std::string> data;  // Key-value store
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expiry; // Expiration times
    std::mutex mtx;  // Mutex for thread safety

public:
    // Set key-value pair
    void set(const std::string& key, const std::string& value);

    // Get value by key
    std::string* get(const std::string& key);

    // Save data to a file
    bool saveToFile(const std::string& filename);

    // Load data from a file
    bool loadFromFile(const std::string& filename);

    // Set expiration for a key (in seconds)
    bool expire(const std::string& key, int seconds);

    // Get the time-to-live (TTL) of a key (returns seconds remaining, -1 for no expiration, -2 for not found)
    int ttl(const std::string& key);

    // Delete a key
    bool del(const std::string& key);

    // Check if a key exists
    bool exists(const std::string& key);

    // Get all keys
    std::vector<std::string> keys();

    // Flush all keys (clear the store)
    void flush();
};

#endif
