#pragma once
#include "ConfigData.h"
#include "TArray.h"
#include "UEngineStatics.h"

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
public:
	static ConfigManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = TUniquePtr<ConfigManager>(new ConfigManager());
		}
		return instance.get();
	}

	ConfigData* GetConfig(const FString& configName) {
		auto itr = configDatas.find(configName);

		if (itr == configDatas.end()) return nullptr;

		return itr->second.get();
	}
};

