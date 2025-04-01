#include "store.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <json/json.hpp>

using json = nlohmann::json;

void Store::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    data[key] = value;
}

std::string* Store::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        return &it->second;
    }
    return nullptr;
}


bool Store::saveToFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx);
    json j;
    for (const auto& pair : data) {
        j[pair.first] = pair.second;
    }

    std::ofstream out(filename);
    if (!out) return false;
    out << j.dump(4);
    return true;
}

bool Store::loadFromFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ifstream in(filename);
    if (!in) return false;

    json j;
    in >> j;

    for (auto& [key, value] : j.items()) {
        data[key] = value;
    }

    return true;
}

bool Store::expire(const std::string& key, int seconds) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        expiry[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
        return true;
    }
    return false;
}

int Store::ttl(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = expiry.find(key);
    if (it != expiry.end()) {
        auto now = std::chrono::steady_clock::now();
        auto expiration_time = it->second;

        if (now >= expiration_time) {
            data.erase(key);
            expiry.erase(it);
            return -2; // Key expired
        }

        auto ttl = std::chrono::duration_cast<std::chrono::seconds>(expiration_time - now).count();
        return static_cast<int>(ttl); // Time remaining
    }
    return -1; // No expiration
}

bool Store::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        data.erase(it);
        expiry.erase(key);  // Remove expiration as well
        return true;
    }
    return false;
}

bool Store::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> keys_list;
    for (const auto& pair : data) {
        keys_list.push_back(pair.first);
    }
    return keys_list;
}

void Store::flush() {
    std::lock_guard<std::mutex> lock(mtx);
    data.clear();
    expiry.clear();
}
