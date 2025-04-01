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