#pragma once

#include <filesystem>
#include <memory>

#include <d3d11.h>

#include "UEngineStatics.h"
#include "ShaderReflector.h"
#include "UObject.h"

class UPixelShader : public UObject
{
public:
	~UPixelShader() = default;

	UPixelShader(
		ID3D11Device* Device,
		const std::filesystem::path& PixelShaderFilePath,
		const FString& PixelShaderMain
	);

	UPixelShader(const UPixelShader&) = delete;
	UPixelShader(UPixelShader&&) noexcept = default;

	UPixelShader& operator=(const UPixelShader&) = delete;
	UPixelShader& operator=(UPixelShader&&) noexcept = default;

	void UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, const FString& BufferName, const void* BufferData) const;

	template<typename... TBufferNames>
	void Bind(ID3D11DeviceContext* DeviceContext, TBufferNames&&... BufferNames) const;

private:
	std::unique_ptr<UShaderReflector> ShaderReflector;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};

// TODO: Incomplete Version
inline void UPixelShader::UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, const FString& BufferName, const void* BufferData) const
{
	auto ConstantBufferInfo = ShaderReflector->GetConstantBufferInfo(BufferName);
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
void UPixelShader::Bind(ID3D11DeviceContext* DeviceContext, TBufferNames && ...BufferNames) const
{
	DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);

	(ShaderReflector->BindPixelShader(DeviceContext, std::forward<TBufferNames>(BufferNames)), ...);
}
