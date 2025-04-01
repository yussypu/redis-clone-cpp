#include "store.hpp"

void Store::set(const std::string& key, const std::string& value) {
    data[key] = value;
}

std::optional<std::string> Store::get(const std::string& key) {
    if (data.find(key) != data.end()) {
        return data[key];
    }
    return std::nullopt;
}

bool Store::del(const std::string& key) {
    return data.erase(key) > 0;
}

bool Store::exists(const std::string& key) {
    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::vector<std::string> all_keys;
    for (const auto& pair : data) {
        all_keys.push_back(pair.first);
    }
    return all_keys;
}

void Store::flush() {
    data.clear();
}
