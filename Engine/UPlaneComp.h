#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UStaticMeshComponent.h"
#include "Vector.h"

class UPlaneComp : public UStaticMeshComponent
{
	DECLARE_UCLASS(UPlaneComp, UStaticMeshComponent)
private:
	bool IsManageable() override { return true; }
public:
	UPlaneComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 });
	~UPlaneComp();
};