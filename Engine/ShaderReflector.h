#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "UEngineStatics.h"
#include "UObject.h"

class UShaderReflector : public UObject
{
public:
	struct FConstantBufferInfo
	{
		UINT Size;
		UINT BindPoint;
		Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	};

public:
	~UShaderReflector() = default;

	UShaderReflector(ID3D11Device* Device, ID3DBlob* ShaderBlob);

	UShaderReflector(const UShaderReflector&) = delete;
	UShaderReflector(UShaderReflector&&) noexcept = default;

	UShaderReflector& operator=(const UShaderReflector&) = delete;
	UShaderReflector& operator=(UShaderReflector&&) noexcept = default;

	FConstantBufferInfo GetConstantBufferInfo(const FString& BufferName);

	void BindVertexShader(ID3D11DeviceContext* DeviceContext, const FString& BufferName);
	void BindPixelShader(ID3D11DeviceContext* DeviceContext, const FString& BufferName);

private:
	TMap<FString, FConstantBufferInfo> ConstantBufferInfoMap;
};
