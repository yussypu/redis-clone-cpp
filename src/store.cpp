#include "store.hpp"
#include <iostream>

Store::Store() {
    startCleanupThread();
}

Store::~Store() {
    running = false;
    if (cleanup_thread.joinable()) {
        cleanup_thread.join();
    }
}

void Store::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    data[key] = value;
    expiry.erase(key); // Remove TTL if it existed
}

std::optional<std::string> Store::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = expiry.find(key);
    if (it != expiry.end() && std::chrono::steady_clock::now() >= it->second) {
        data.erase(key);
        expiry.erase(key);
        return std::nullopt;
    }

    if (data.find(key) != data.end()) {
        return data[key];
    }
    return std::nullopt;
}

bool Store::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    expiry.erase(key);
    return data.erase(key) > 0;
}

bool Store::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = expiry.find(key);
    if (it != expiry.end() && std::chrono::steady_clock::now() >= it->second) {
        data.erase(key);
        expiry.erase(key);
        return false;
    }

    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> all_keys;

    for (const auto& pair : data) {
        all_keys.push_back(pair.first);
    }

    return all_keys;
}

void Store::flush() {
    std::lock_guard<std::mutex> lock(mtx);
    data.clear();
    expiry.clear();
}

bool Store::expire(const std::string& key, int seconds) {
    std::lock_guard<std::mutex> lock(mtx);
    if (data.find(key) == data.end()) return false;
    expiry[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}

int Store::ttl(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);

    if (data.find(key) == data.end()) return -2;

    auto it = expiry.find(key);
    if (it == expiry.end()) return -1;

    auto now = std::chrono::steady_clock::now();
    auto expires_at = it->second;

    if (now >= expires_at) {
        data.erase(key);
        expiry.erase(key);
        return -2;
    }

    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(expires_at - now).count();
    return static_cast<int>(remaining);
}


void Store::startCleanupThread() {
    cleanup_thread = std::thread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            cleanupExpiredKeys();
        }
    });
}

void Store::cleanupExpiredKeys() {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();

    for (auto it = expiry.begin(); it != expiry.end(); ) {
        if (now >= it->second) {
            data.erase(it->first);
            it = expiry.erase(it);
        } else {
            ++it;
        }
    }
}
