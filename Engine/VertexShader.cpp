#include <d3d11.h>
#include <d3dcompiler.h>


#include "stdafx.h"
#include "UEngineStatics.h"
#include "VertexShader.h"
#include "TArray.h"

UVertexShader::UVertexShader(
	ID3D11Device* Device, 
	const std::filesystem::path& VertexShaderFilePath,
	const FString& VertexShaderMain,
	const TArray<D3D11_INPUT_ELEMENT_DESC>& InputLayoutDesc
)
{
	if (!std::filesystem::exists(VertexShaderFilePath))
	{
		throw std::invalid_argument("File not exist: " + VertexShaderFilePath.string());
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> ShaderErrorBlob;

	HRESULT hResult = D3DCompileFromFile(
		VertexShaderFilePath.wstring().c_str(),
		nullptr,
		nullptr,
		VertexShaderMain.c_str(),
		"vs_5_0",
		0,
		0,
		ShaderBlob.ReleaseAndGetAddressOf(),
		ShaderErrorBlob.ReleaseAndGetAddressOf()
	);

	// TODO: Improve Error Handling
	if (FAILED(hResult))
	{
		if (ShaderErrorBlob)
		{
			throw std::runtime_error(
				"Vertex Shader Compilation Failed: " +
				std::string(static_cast<const char*>(ShaderErrorBlob->GetBufferPointer()))
			);
		}
		else
		{
			throw std::runtime_error("Vertex Shader Compilation Failed");
		}
	}

	Device->CreateVertexShader(
		ShaderBlob->GetBufferPointer(),
		ShaderBlob->GetBufferSize(),
		nullptr,
		VertexShader.ReleaseAndGetAddressOf()
	);

	ShaderReflector = std::make_unique<UShaderReflector>(Device, ShaderBlob.Get());

	Device->CreateInputLayout(
		InputLayoutDesc.data(),
		InputLayoutDesc.size(),
		ShaderBlob->GetBufferPointer(),
		ShaderBlob->GetBufferSize(),
		InputLayout.ReleaseAndGetAddressOf()
	);
}
