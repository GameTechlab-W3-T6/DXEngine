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
	UStaticMeshComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl) {
	}
	void InitializeUUID()
	{
		uuidQuad.Init();
		// uuidQuad.Initialize(textureManager, meshManager);
		uuidQuad.SetText(UUID);
	}
	/*virtual void Draw(URenderer& renderer) override;*/

private:
	bool initialized = false;
};

