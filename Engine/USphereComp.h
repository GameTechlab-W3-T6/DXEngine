#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

/**
 * @brief Sphere primitive component for rendering sphere meshes
 */
class USphereComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(USphereComp, UPrimitiveComponent)
private:
	bool IsManageable() override { return true; }
public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
		Name = GetDefaultName();
		ID = SphereID++;
	}

	virtual uint32 GetID() const { return ID;  }

protected:
	virtual const char* GetDefaultName() const override { return "Sphere"; }

private:
	static uint32 SphereID;
	uint32 ID;
};