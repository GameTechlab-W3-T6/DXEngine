#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "Vector.h"

/**
 * @brief Plane primitive component for rendering plane meshes
 */
class UPlaneComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UPlaneComp, UPrimitiveComponent)
private:
	bool IsManageable() override { return true; }
public:
	UPlaneComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 });

	virtual ~UPlaneComp();

	virtual uint32 GetID() const { return ID;  }

protected:
	virtual const char* GetDefaultName() const override { return "Plane"; }

private:
	static uint32 PlaneID;
	uint32 ID;
};