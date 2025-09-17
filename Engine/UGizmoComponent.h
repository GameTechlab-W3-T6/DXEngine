#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Shader.h"
#include "UPrimitiveComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언

/**
 * @brief Interactive gizmo component for editor manipulation
 */
class UGizmoComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(UGizmoComponent, UPrimitiveComponent)
public:
	FQuaternion OriginQuaternion;

	UGizmoComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: UPrimitiveComponent(loc, rot, scl)
	{
	}

	bool bIsSelected = false;

	bool CountOnInspector() override { return true; }

	FMatrix GetWorldTransform() override;

	virtual void Update(float deltaTime);
	
	bool Initialize() override;

	virtual void UpdateConstantBuffer(URenderer& renderer) override;

	virtual void BindVertexShader(URenderer& renderer) override;

	virtual void BindPixelShader(URenderer& renderer) override;

	virtual void Draw(URenderer& renderer) override;

	virtual LayerID GetLayer() const override { return 0; } 

	void DrawOnTop(URenderer& renderer);

	void SetOriginRotation(FVector originRotation)
	{
		OriginQuaternion = FQuaternion::FromEulerXYZDeg(originRotation);
	}

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { 
		if (bIsSelected)
			return FVector4(1.0f, 1.0f, 0.0f, 1.0f);
		return Color; 
	}

protected:
	FVector4 Color = { 1, 1, 1, 1 };
};
