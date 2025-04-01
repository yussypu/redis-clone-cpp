#pragma once
#include <string>
#include <unordered_map>
#include <optional>
#include <vector>

class Store {
public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    std::vector<std::string> keys();
    void flush();
private:
    std::unordered_map<std::string, std::string> data;
};