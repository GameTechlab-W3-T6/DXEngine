#pragma once

#include <cassert>
#include <filesystem>

#include <d3d11.h>

#include "TArray.h"
#include "UClass.h"
#include "UEngineSubsystem.h"
#include "UEngineStatics.h"

#include "Shader.h"

class UBatchShaderManager : public UEngineSubsystem
{
	DECLARE_UCLASS(UBatchShaderManager, UEngineSubsystem)
public:
	/** @todo */
	using ShaderID = uint8_t;

	virtual ~UBatchShaderManager() = default;

	UBatchShaderManager() = default;

	UBatchShaderManager(const UBatchShaderManager&) = delete;
	UBatchShaderManager(UBatchShaderManager&&) = delete;

	UBatchShaderManager& operator=(const UBatchShaderManager&) = delete;
	UBatchShaderManager& operator=(UBatchShaderManager&&) = delete;

	void Initialize(ID3D11Device* Device);

	void LoadShaderFromFile(ID3D11Device* Device, EShaderType ShaderType, const std::filesystem::path& FilePath, const FString& EntryPoint, const FString& Name = "");

	ShaderID GetShaderIDByName(const FString& Name)
	{
		auto it = ShaderIndexMap.find(Name);

		assert(it != ShaderIndexMap.end());

		return it->second;
	}

	UShader* GetShaderByName(const FString& Name)
	{
		return GetShaderByID(GetShaderIDByName(Name));
	}

	UShader* GetShaderByID(ShaderID ID)
	{
		assert(ID < ShaderArray.size());

		return ShaderArray[ID].get();
	}

private:
	TArray<TUniquePtr<UShader>> ShaderArray;
	TMap<FString, size_t> ShaderIndexMap;
};