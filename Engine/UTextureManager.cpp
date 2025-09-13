#include "stdafx.h"
#include "UTextureManager.h"

#pragma comment(lib, "DirectXTK.lib")
#include "DDSTextureLoader.h"  
#include "WICTextureLoader.h"
#include "FTextInfo.h"
IMPLEMENT_UCLASS(UTextureManager, UEngineSubsystem)


bool UTextureManager::Initialize(URenderer* renderer)
{  
	textures["TextInfo"] = LoadFromFile(renderer->GetDevice(), renderer->GetDeviceContext(), L".\\Textures\\fire.png", false);

	textures["PlaneBaseColor"] = LoadFromFile(renderer->GetDevice(), renderer->GetDeviceContext(), L".\\Textures\\fire.png", false);
	
	BindPS(renderer->GetDeviceContext(), textures["TextInfo"], 0, 0);
	return true;
}


FTexture* UTextureManager::LoadFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	const std::wstring& path, bool isDDS)
{ 
	FTexture* texture = new FTexture();  
	ID3D11Resource* resource = nullptr;

	HRESULT hr;
	if (isDDS)
	{
		hr = DirectX::CreateDDSTextureFromFile(device, path.c_str(), &resource, &(texture->srv));
	}

	else
	{
		hr = DirectX::CreateWICTextureFromFile(device, path.c_str(), &resource, &(texture->srv));
	}

	if (FAILED(hr))
	{
		UE_LOG("Failed to load texture: %s \n", path.c_str());
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, &(texture->samplerState));
	
	if (resource)
	{
		ID3D11Texture2D* tex2D = nullptr;
		hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex2D); 
		if (SUCCEEDED(hr) && tex2D)
		{
			D3D11_TEXTURE2D_DESC desc;
			tex2D->GetDesc(&desc);

			texture->width = desc.Width;
			texture->height = desc.Height;

			tex2D->Release();
		}

		resource->Release();  
	} 

	return texture;
}
FTexture* UTextureManager::RetrieveTexture(FString textureName)
{
	auto itr = textures.find(textureName);
	if (itr == textures.end()) return nullptr;
	return itr->second;
}
FTexture* UTextureManager::CreateTextureInternal()
{ 
	return nullptr;
}


void UTextureManager::BindPS(ID3D11DeviceContext* deviceContext, FTexture* texture, UINT textureSlot, UINT samplerSlot)
{
	deviceContext->PSSetShaderResources(textureSlot, 1, &(texture->srv));
	
	if(samplerSlot >= 0)
		deviceContext->PSSetSamplers(samplerSlot, 1, &(texture->samplerState));
}

void UTextureManager::UnBindPS(ID3D11DeviceContext* deviceContext, UINT textureSlot, UINT samplerSlot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ID3D11SamplerState* nullSampler[1] = { nullptr };

	deviceContext->PSSetShaderResources(textureSlot, 1, nullSRV);
	deviceContext->PSSetSamplers(samplerSlot, 1, nullSampler);
}

void UTextureManager::CallCharacter(TArray<FTextInfo*>& stringList, UClass* cl , int key)
{ 
	////TODO: 정리
	FTextInfo* textInfo = new FTextInfo();
	FTexture* textTex = RetrieveTexture(GetClass()->GetMeta("TextInfo"));
	textInfo->keyCode = key;
	textInfo->textTexture = textTex;
	textInfo->cellsPerColumn = 16;
	textInfo->cellsPerRow = 16;
	textInfo->cellWidth = textTex->width / textInfo->cellsPerRow;
	textInfo->cellHeight = textTex->height / textInfo->cellsPerColumn;
	
	stringList.push_back(textInfo); 

}




UTextureManager::UTextureManager()
{
	//textures["fire"] = CreateTextureInternal();
}

UTextureManager::~UTextureManager()
{
}

