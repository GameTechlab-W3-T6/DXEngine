#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UStaticMeshComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

/**
 * @brief Cube primitive component for rendering cube meshes
 */
class UCubeComp : public UStaticMeshComponent
{
	DECLARE_UCLASS(UCubeComp, UStaticMeshComponent)
private:
	static inline FString type = "Cube";

	static USceneComponent* Create()
	{
		USceneComponent* newInstance = new UCubeComp();

		return newInstance;
	}

	bool IsManageable() override { return true; }

public:
	UCubeComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UStaticMeshComponent(pos, rot, scl)
	{
	}
};