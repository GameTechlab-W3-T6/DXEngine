#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

class IniFile {
public:
    std::map<std::string, std::map<std::string, std::string>> data;

    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line, currentSection;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;

            if (line.front() == '[' && line.back() == ']') {
                currentSection = trim(line.substr(1, line.size() - 2));
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            toLower(key);
            data[currentSection][key] = value;
        }
        return true;
    }

    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const {
        auto secIt = data.find(section);
        if (secIt != data.end()) {
            auto keyIt = secIt->second.find(toLowerCopy(key));
            if (keyIt != secIt->second.end()) {
                return keyIt->second;
            }
        }
        return defaultValue;
    }

    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const {
        std::string val = getString(section, key);
        if (val.empty()) return defaultValue;
        return std::stoi(val);
    }

    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const {
        std::string val = getString(section, key);
        toLower(val);
        if (val == "true" || val == "1" || val == "yes" || val == "on") return true;
        if (val == "false" || val == "0" || val == "no" || val == "off") return false;
        return defaultValue;
    }

private:
    static inline std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    static inline void toLower(std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    }

    static inline std::string toLowerCopy(std::string s) {
        toLower(s);
        return s;
    }
};

