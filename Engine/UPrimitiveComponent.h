#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"
#include "Vector.h"
#include "UClass.h" 
#include "FTextInfo.h"
class UMeshManager; // 전방 선언
class UTextureManager;
class UCamera;
struct FTexture;

class UPrimitiveComponent : public USceneComponent
{
	DECLARE_UCLASS(UPrimitiveComponent, USceneComponent)
protected:
	UMesh* mesh;
	FTexture* texture;
	
	FTextInfo* textInfo;
	UCamera* camera;
	FVector4 Color = { 1, 1, 1, 1 };
	
	//임시
	UInputManager* inputManager;
	UTextureManager* textureManager;

public:
	UPrimitiveComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr), textInfo(new FTextInfo())
	{}
	virtual ~UPrimitiveComponent() {}
	
	bool bIsSelected = false;

	// 별도의 초기화 메서드
	virtual bool Init(UMeshManager* meshManager, UInputManager* im, UTextureManager* tm = nullptr, UCamera* cam = nullptr);
	virtual void Draw(URenderer& renderer);
	virtual void UpdateConstantBuffer(URenderer& renderer); 

	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return mesh; }

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { return Color; }
};
