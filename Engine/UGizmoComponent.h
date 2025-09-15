#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Shader.h"
#include "USceneComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언

/**
 * @brief Interactive gizmo component for editor manipulation
 */
class UGizmoComponent : public USceneComponent
{
	DECLARE_UCLASS(UGizmoComponent, USceneComponent)
public:
	FQuaternion OriginQuaternion;

	UGizmoComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr)
	{
	}

	bool bIsSelected = false;

	bool Init();
	bool CountOnInspector() override { return true; }

	FMatrix GetWorldTransform() override;

	virtual void Update(float deltaTime);

	virtual void UpdateConstantBuffer(URenderer& renderer);

	virtual void BindVertexShader(URenderer& renderer);

	virtual void BindPixelShader(URenderer& renderer);

	void BindShader(URenderer& renderer);

	void BindMesh(URenderer& renderer);

	virtual void Draw(URenderer& renderer);

	virtual void DrawOnTop(URenderer& renderer);
	

	UMesh* GetMesh() { return mesh; }

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
	UMesh* mesh;
	UShader* vertexShader, * pixelShader;
	FVector4 Color = { 1, 1, 1, 1 };
};
