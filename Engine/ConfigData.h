#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include "UEngineStatics.h"

class ConfigData {
public:
    std::map<FString, std::map<FString, FString>> data;

    ConfigData(const std::filesystem::path& path) {
        configPath = path;

        // If .ini file doesn't exist, try to copy from .default.ini
        if (!std::filesystem::exists(configPath)) {
            std::filesystem::path defaultPath = configPath;
            defaultPath.replace_extension(".default.ini");

            if (std::filesystem::exists(defaultPath)) {
                try {
                    std::filesystem::copy_file(defaultPath, configPath);
                } catch (const std::filesystem::filesystem_error& e) {
                    throw std::runtime_error("Failed to copy default config file: " + FString(e.what()));
                }
            }
        }

        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open config file: " + configPath.string());
        }

        FString line, currentSection;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;

            if (line.front() == '[' && line.back() == ']') {
                currentSection = trim(line.substr(1, line.size() - 2));
                continue; 
            }

            auto pos = line.find('=');
            if (pos == FString::npos) continue;

            FString key = trim(line.substr(0, pos));
            FString value = trim(line.substr(pos + 1));

            data[currentSection][key] = value;
        }
    }

    ~ConfigData() noexcept
    {
        try {
            std::ofstream file(configPath);
            if (!file.is_open()) {
                return;
            }
            for (const auto& section : data)
            {
                file << "[" << section.first << "]" << std::endl;
                for (const auto& field : section.second)
                {
                    file << field.first << " = " << field.second << std::endl;
                }
                file << std::endl;
            }
        }
        catch (...) {
        }
    }

    FString getString(const FString& section, const FString& key, const FString& defaultValue = "") const {
        auto secIt = data.find(section);
        if (secIt != data.end()) {
            auto keyIt = secIt->second.find(key);
            if (keyIt != secIt->second.end()) {
                return keyIt->second;
            }
        }
        return defaultValue;
    }

    int getInt(const FString& section, const FString& key, int defaultValue = 0) const {
        FString val = getString(section, key);
        if (val.empty()) return defaultValue;
        return std::stoi(val);
    }

    float getFloat(const FString& section, const FString& key, float defaultValue = 0.0f) const {
        FString val = getString(section, key);
        if (val.empty()) return defaultValue;
        return std::stof(val);
    }

    bool getBool(const FString& section, const FString& key, bool defaultValue = false) const {
        FString val = getString(section, key);
        toLower(val);
        if (val == "true" || val == "1" || val == "yes" || val == "on") return true;
        if (val == "false" || val == "0" || val == "no" || val == "off") return false;
        return defaultValue;
    }

    void setString(const FString& section, const FString& key, const FString& value)
    {
        data[section][key] = value;
    }
    
    void setInt(const FString& section, const FString& key, const int value)
    {
        setString(section, key, std::to_string(value));
    }

    void setFloat(const FString& section, const FString& key, const float value)
    {
        setString(section, key, std::to_string(value));
    }

    void setBool(const FString& section, const FString& key, const bool value)
    {
        setString(section, key, value ? "true" : "false");
    }

private:
    std::filesystem::path configPath;
    static inline FString trim(const FString& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        return (start == FString::npos) ? "" : s.substr(start, end - start + 1);
    }

    static inline void toLower(FString& s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    }
};

