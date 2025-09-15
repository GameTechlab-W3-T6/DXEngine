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
	
	// TODO : textInfo 없애기
	FTextInfo* textInfo;
	UCamera* camera;
	FVector4 Color = { 1, 1, 0, 1 };
	
	// TODO : 제발제발제발 임시
	UInputManager* inputManager;
	UTextureManager* textureManager;
	UMeshManager* meshManager;
	URenderer* renderer;
	
public:
	UPrimitiveComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr), textInfo(new FTextInfo())
	{}
	virtual ~UPrimitiveComponent() {}

	// 해당 primitive 활성화 여부
	bool bVisible = true;
	bool bIsSelected = false;

	virtual bool Init(URenderer* rd, UMeshManager* mM, UInputManager* im, UTextureManager* tm, UCamera* cam);
	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled);
	virtual void UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture,  bool bIsShaderReflectionEnabled);

	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return mesh; }
	FTexture* GetTextTexture() { return textInfo->textTexture; }

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { return Color; }
};
