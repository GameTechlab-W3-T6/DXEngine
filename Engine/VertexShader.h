#pragma once

#include <filesystem>
#include <memory>

#include <d3d11.h>

#include "UEngineStatics.h"
#include "ShaderReflector.h"
#include "TArray.h"
#include "UObject.h"

class UVertexShader : public UObject
{
public:
	~UVertexShader() = default;

	UVertexShader(
		ID3D11Device* Device,
		const std::filesystem::path& VertexShaderFilePath,
		const FString& VertexShaderMain,
		const TArray<D3D11_INPUT_ELEMENT_DESC>& InputLayoutDesc
	);

	UVertexShader(const UVertexShader&) = delete;
	UVertexShader(UVertexShader&&) noexcept = default;

	UVertexShader& operator=(const UVertexShader&) = delete;
	UVertexShader& operator=(UVertexShader&&) noexcept = default;

		void UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, const FString& BufferName, const void* BufferData) const;

	template<typename... TBufferNames>
	void Bind(ID3D11DeviceContext* DeviceContext, TBufferNames&&... BufferNames) const;

private:
	std::unique_ptr<UShaderReflector> ShaderReflector;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
};

// TODO: Incomplete Version
inline void UVertexShader::UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, const FString& BufferName, const void* BufferData) const
{
	UShaderReflector::FConstantBufferInfo ConstantBufferInfo;
	// TODO
	try
	{
		ConstantBufferInfo = ShaderReflector->GetConstantBufferInfo(BufferName);
	}
	catch (...)
	{
		return;
	}
	//
	DeviceContext->UpdateSubresource(
		ConstantBufferInfo.ConstantBuffer.Get(),
		0, 
		nullptr, 
		BufferData, 
		0,
		0
	);
}

template<typename ...TBufferNames>
void UVertexShader::Bind(ID3D11DeviceContext* DeviceContext, TBufferNames && ...BufferNames) const
{
	DeviceContext->IASetInputLayout(InputLayout.Get());

	DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);

	(ShaderReflector->BindVertexShader(DeviceContext, std::forward<TBufferNames>(BufferNames)), ...);
}
