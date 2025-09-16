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

	static std::map<FString, FString>& GetSection(const FString& configName, const FString& sectionName)
	{
		static std::map<FString, FString> emptySection;

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
			config->data[sectionName] = std::map<FString, FString>();
			return config->data[sectionName];
		}

		return it->second;
	}

	static FString GetValue(const FString& configName, const FString& sectionName, const FString& key, const FString& defaultValue = "")
	{
		ConfigData* config = GetConfig(configName);
		if (!config)
		{
			return defaultValue;
		}

		return config->getString(sectionName, key, defaultValue);
	}
};

