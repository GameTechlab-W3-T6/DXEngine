#pragma once
#include "ConfigData.h"
#include "TArray.h"
#include "UEngineStatics.h"
#include <stdexcept>

class ConfigManager
{
private:
	static inline TUniquePtr<ConfigManager> instance;
	TMap<FString, TUniquePtr<ConfigData>> configDatas;
	std::filesystem::path rootPath = std::filesystem::current_path();
	TArray<std::filesystem::path> configFiles = {
		"editor.ini"
	};
	ConfigManager()
	{
		for (const auto& configFilePath : configFiles)
		{
			configDatas[configFilePath.stem().string()] = MakeUnique<ConfigData>(rootPath / configFilePath);
		}
	}
	ConfigData* GetConfigInternal(const FString& configName) {
		auto itr = configDatas.find(configName);

		if (itr == configDatas.end()) return nullptr;

		return itr->second.get();
	}
public:
	/** @todo: Change instance into local static variable. */
	static ConfigManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = TUniquePtr<ConfigManager>(new ConfigManager());
		}
		return instance.get();
	}

	static ConfigData* GetConfig(const FString& configName)
	{
		return GetInstance()->GetConfigInternal(configName);
	}

	static std::map<std::string, std::string>& GetSection(const FString& configName, const std::string& sectionName)
	{
		static std::map<std::string, std::string> emptySection;

		ConfigData* config = GetConfig(configName);
		if (!config)
		{
			emptySection.clear();
			return emptySection;
		}

		auto it = config->data.find(sectionName);
		if (it == config->data.end())
		{
			// Create new section if it doesn't exist
			config->data[sectionName] = std::map<std::string, std::string>();
			return config->data[sectionName];
		}

		return it->second;
	}

	static std::string GetValue(const FString& configName, const std::string& sectionName, const std::string& key, const std::string& defaultValue = "")
	{
		ConfigData* config = GetConfig(configName);
		if (!config)
		{
			return defaultValue;
		}

		return config->getString(sectionName, key, defaultValue);
	}
};

