#pragma once
#include "stdafx.h"
#include "UGizmoComponent.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class URenderer;

class UGizmoGridComp : public UGizmoComponent
{
	DECLARE_UCLASS(UGizmoGridComp, UGizmoComponent)
public:
	using UGizmoComponent::UGizmoComponent;
	
	json::JSON Serialize() const override
	{
		return json::JSON();
	}

	UGizmoGridComp();

	void Update(float deltaTime) override;
};