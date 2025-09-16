#pragma once

#include "FTexture.h"
#include "URenderer.h"
#include "UEngineSubsystem.h"


class UTextureManager : public UEngineSubsystem
{
	DECLARE_UCLASS(UTextureManager, UEngineSubsystem)

public:
	bool Initialize(URenderer* renderer);

	FTexture* RetrieveTexture(FString textureName);

	void BindPS(ID3D11DeviceContext* deviceContext, FTexture* texture, UINT textureSlot, UINT samplerSlot = -1);
	void UnBindPS(ID3D11DeviceContext* deviceContext, UINT textureSlot = 0, UINT samplerSlot = 0);

	// text atlas
	void CallCharacter(TArray<FTextInfo*>& stringList, UClass* cl, int key);
private:
	TMap<FString, FTexture*> textures;

	FTexture* LoadFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const std::wstring& path, bool isDDS);


	FTexture* CreateTextureInternal();


public:
	UTextureManager();
	~UTextureManager();
};