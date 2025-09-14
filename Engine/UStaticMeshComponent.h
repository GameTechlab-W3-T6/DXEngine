#pragma once
#include "UPrimitiveComponent.h"
#include "UTextholderComp.h"

// ShapeComponent만 Textholder
class UStaticMeshComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(UStaticMeshComponent, UPrimitiveComponent)
protected:
	UTextholderComp uuidQuad;
	UStaticMeshComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
		uuidQuad.SetText(UUID);
	}
	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
};

