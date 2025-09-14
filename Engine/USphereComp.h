#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UStaticMeshComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class USphereComp : public UStaticMeshComponent
{
	DECLARE_UCLASS(USphereComp, UStaticMeshComponent)
private:
	bool IsManageable() override { return true; }
public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UStaticMeshComponent(pos, rot, scl)
	{
	}
};