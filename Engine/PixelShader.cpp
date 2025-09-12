#include <d3d11.h>
#include <d3dcompiler.h>

#include "stdafx.h"
#include "PixelShader.h"

UPixelShader::UPixelShader(
	ID3D11Device* Device, 
	const std::filesystem::path& PixelShaderFilePath, 
	const FString& PixelShaderMain
)
{
	if (!std::filesystem::exists(PixelShaderFilePath))
	{
		throw std::invalid_argument("File not exist: " + PixelShaderFilePath.string());
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> ShaderErrorBlob;

	HRESULT hResult = D3DCompileFromFile(
		PixelShaderFilePath.wstring().c_str(),
		nullptr,
		nullptr,
		PixelShaderMain.c_str(),
		"ps_5_0",
		0,
		0,
		ShaderBlob.GetAddressOf(),
		ShaderErrorBlob.GetAddressOf()
	);

	// TODO: Improve Error Handling
	if (FAILED(hResult))
	{
		throw std::runtime_error("Pixel Shader Compilation Failed");
	}

	Device->CreatePixelShader(
		ShaderBlob->GetBufferPointer(),
		ShaderBlob->GetBufferSize(),
		nullptr,
		PixelShader.GetAddressOf()
	);

	ShaderReflector = std::make_unique<UShaderReflector>(Device, ShaderBlob.Get());
}
