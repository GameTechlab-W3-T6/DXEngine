#pragma once

#include <cassert>
#include <stdexcept>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <wrl.h>

#include "stdafx.h"
#include "ShaderReflector.h"

UShaderReflector::UShaderReflector(ID3D11Device* Device, ID3DBlob* ShaderBlob)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> ShaderReflector;
	D3DReflect(
		ShaderBlob->GetBufferPointer(),
		ShaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(ShaderReflector.ReleaseAndGetAddressOf())
	);

	D3D11_SHADER_DESC ShaderDesc;
	ShaderReflector->GetDesc(&ShaderDesc);

	for (UINT i = 0; i < ShaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* ConstantBufferReflector
			= ShaderReflector->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
		ConstantBufferReflector->GetDesc(&ShaderBufferDesc);

		D3D11_SHADER_INPUT_BIND_DESC ShaderInputBindDesc;
		ShaderReflector->GetResourceBindingDescByName(
			ShaderBufferDesc.Name,
			&ShaderInputBindDesc
		);

		D3D11_BUFFER_DESC ConstantBufferDesc = {};
		ConstantBufferDesc.ByteWidth = ShaderBufferDesc.Size;
		ConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = 0;
		ConstantBufferDesc.MiscFlags = 0;
		ConstantBufferDesc.StructureByteStride = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
		Device->CreateBuffer(
			&ConstantBufferDesc,
			nullptr,
			ConstantBuffer.ReleaseAndGetAddressOf()
		);

		FConstantBufferInfo ConstantBufferInfo = {};
		ConstantBufferInfo.Size = ShaderBufferDesc.Size;
		ConstantBufferInfo.BindPoint = ShaderInputBindDesc.BindPoint;
		ConstantBufferInfo.ConstantBuffer = ConstantBuffer;
		assert(!ConstantBufferInfoMap.count(ShaderBufferDesc.Name));
		ConstantBufferInfoMap[ShaderBufferDesc.Name] = ConstantBufferInfo;
	}
}

UShaderReflector::FConstantBufferInfo 
	UShaderReflector::GetConstantBufferInfo(const FString& BufferName)
{
	// TODO: Improve Error Handling
	if (!ConstantBufferInfoMap.count(BufferName))
	{
		throw std::invalid_argument(BufferName + " not found");
	}
	else
	{
		return ConstantBufferInfoMap[BufferName];
	}
}

void UShaderReflector::BindVertexShader(ID3D11DeviceContext* DeviceContext, const FString& BufferName)
{
	auto ConstantBufferInfo = GetConstantBufferInfo(BufferName);
	DeviceContext->VSSetConstantBuffers(
		ConstantBufferInfo.BindPoint,
		1,
		ConstantBufferInfo.ConstantBuffer.GetAddressOf()
	);
}

void UShaderReflector::BindPixelShader(ID3D11DeviceContext* DeviceContext, const FString& BufferName)
{
	auto ConstantBufferInfo = GetConstantBufferInfo(BufferName);
	DeviceContext->PSSetConstantBuffers(
		ConstantBufferInfo.BindPoint,
		1,
		ConstantBufferInfo.ConstantBuffer.GetAddressOf()
	);
}
