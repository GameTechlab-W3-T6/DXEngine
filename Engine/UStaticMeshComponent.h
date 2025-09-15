#pragma once
#include "UPrimitiveComponent.h"
#include "UTextholderComp.h"

// ShapeComponent만 Textholder
class UStaticMeshComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(UStaticMeshComponent, UPrimitiveComponent)

protected:
	UTextholderComp uuidQuad;

public:
	void InitializeUUID()
	{
		uuidQuad.Init(renderer, meshManager, inputManager, textureManager, camera);
		// uuidQuad.Initialize(textureManager, meshManager);
		uuidQuad.SetText(UUID);
	}

protected:
	UStaticMeshComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl) {
	}

	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;

private:
	bool initialized = false;
};

