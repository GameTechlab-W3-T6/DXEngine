#pragma once
#include "UPrimitiveComponent.h"
#include "UTextholderComp.h"

// ShapeComponent만 Textholder
class UStaticMeshComponent : public UPrimitiveComponent
{
private:
	UStaticMeshComponent();
	UTextholderComp uuidQuad;

	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
};

