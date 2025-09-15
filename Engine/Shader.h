#pragma once

#include <filesystem>
#include <memory>

#include <d3d11.h>

#include "ShaderReflection.h"
#include "UEngineStatics.h"

// TODO: Change names

class UShader
{
public:
	using ShaderID = uint8_t;

	~UShader() = default;

	UShader(ID3D11Device* Device, ShaderID ID, EShaderType InShaderType, const std::filesystem::path& FilePath, const FString& EntryPoint)
		: ShaderType(InShaderType)
	{
		if (!std::filesystem::exists(FilePath))
		{
			// UE_LOG();
			throw std::runtime_error("File not found: " + FilePath.string());
		}

		Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderErrorBlob;

		const char* ShaderModel = nullptr;
		switch (InShaderType)
		{
		case EShaderType::VertexShader:
			ShaderModel = "vs_5_0";
			break;
		case EShaderType::PixelShader:
			ShaderModel = "ps_5_0";
			break;
		default:
			assert(false && "Unsupported shader type.");
			return;
		}

		HRESULT hResult = D3DCompileFromFile(
			FilePath.wstring().c_str(),
			nullptr,
			nullptr,
			EntryPoint.c_str(),
			ShaderModel,
			0,
			0,
			ShaderBlob.GetAddressOf(),
			ShaderErrorBlob.GetAddressOf()
		);

		if (FAILED(hResult))
		{
			if (ShaderErrorBlob)
			{
				OutputDebugStringA("Vertex Shader Compile Error:\n");
				OutputDebugStringA((char*)ShaderErrorBlob->GetBufferPointer());
			}
			else
			{
				OutputDebugStringA("Failed to load vertex shader file: ShaderW0.vs\n");
			}
			assert(false);
		}

		/** Shader Reflection */
		ShaderReflection = MakeUnique<UShaderReflection>(Device, ShaderBlob.Get(), InShaderType);

		switch (InShaderType)
		{
		case EShaderType::VertexShader:
			Device->CreateVertexShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf());
			break;
		case EShaderType::PixelShader:
			Device->CreatePixelShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf());
			break;
		}
	}

	/** Deleted move/copy constructor */
	UShader(const UShader&) = delete;
	UShader(UShader&&) = delete;

	/** Deleted move/copy assignment operator */
	UShader& operator=(const UShader&) = delete;
	UShader& operator=(UShader&&) = delete;

	UBufferElement operator[](const char* Name)
	{
		return ShaderReflection->GetConstantDynamicBuffer(FString(Name))[0];
	}

	/** Get dynamic buffer corresponds to cbuffer by its name */
	UBufferElement operator[](const FString& Name)
	{
		return ShaderReflection->GetConstantDynamicBuffer(Name)[0];
	}

	ShaderID GetID() const
	{
		return ID;
	}

	void BindConstantBuffer(ID3D11DeviceContext* DeviceContext, const FString& BufferName)
	{
		ShaderReflection->Bind(DeviceContext, BufferName);
	}

	template<typename... TBufferNames>
	void BindConstantBuffers(ID3D11DeviceContext* DeviceContext, TBufferNames&&... BufferNames)
	{
		(ShaderReflection->Bind(DeviceContext, std::forward<TBufferNames>(BufferNames)), ...);
	}

	template<typename... TBufferNames>
	void Bind(ID3D11DeviceContext* DeviceContext, TBufferNames&&... BufferNames)
	{
		switch (ShaderType)
		{
		case EShaderType::VertexShader:
			DeviceContext->IASetInputLayout(ShaderReflection->GetInputLayout());
			DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
			break;
		case EShaderType::PixelShader:
			DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
			break;
		default:
			assert(false && "Unsupported shader type.");
			break;
		}

		/** @todo: Remove? */
		BindConstantBuffers(DeviceContext, std::forward<TBufferNames>(BufferNames)...);
	}

private:
	ShaderID ID;

	EShaderType ShaderType;

	/** Composite Shader Reflection class. Unique pointer for creating it inside constructor. */
	TUniquePtr<UShaderReflection> ShaderReflection;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};