// PCH
#include "stdafx.h"

// UObject Headers
#include "UClass.h"
#include "UEngineStatics.h"
#include "UEngineSubsystem.h"
#include "UBatchShaderManager.h"

#include "Shader.h"

IMPLEMENT_UCLASS(UBatchShaderManager, UEngineSubsystem)

void UBatchShaderManager::Initialize(ID3D11Device* Device)
{
	/** @todo: Remove this. */
	LoadShaderFromFile(Device, EShaderType::VertexShader, "ShaderW0.vs", "main", "Vertex");
	LoadShaderFromFile(Device, EShaderType::PixelShader, "ShaderW0.ps", "main", "Pixel");
}

void UBatchShaderManager::LoadShaderFromFile(ID3D11Device* Device, EShaderType ShaderType, const std::filesystem::path& FilePath, const FString& EntryPoint, const FString& Name)
{
	// TODO : looks like need refactor?
	static uint8_t nextShaderId = 2;
	ShaderArray.emplace_back(MakeUnique<UShader>(Device, nextShaderId++, ShaderType, FilePath, EntryPoint));

	/** @note: Size of ShaderArray should be lower than maximum size of ShaderID type. */
	assert(ShaderArray.size() < (1 << (sizeof(ShaderID) * 8)));

	if (!Name.empty())
	{
		ShaderIndexMap[Name] = ShaderArray.size() - 1;
	}
}