#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Shader.h"
#include "USceneComponent.h"
#include "Vector.h"
#include "UClass.h"
#include "ConfigData.h"
#include "ConfigManager.h"

class UMeshManager; // 전방 선언


/**
 * @brief Renderable component with mesh and material properties
 */
class UPrimitiveComponent : public USceneComponent
{
	DECLARE_UCLASS(UPrimitiveComponent, USceneComponent)
protected:
	UMesh* mesh;
	UShader* vertexShader, *pixelShader;
	FVector4 Color = { 1, 1, 1, 1 };
	bool cachedIsShaderReflectionEnabled;
public:
	UPrimitiveComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr), vertexShader(nullptr),  pixelShader(nullptr)
	{
		ConfigData* editorConfig = ConfigManager::GetConfig("editor");
		cachedIsShaderReflectionEnabled = editorConfig->getBool("Graphics", "ShaderReflection");
	}

	bool bIsSelected = false;

	virtual void Draw(URenderer& renderer);
	virtual void UpdateConstantBuffer(URenderer& renderer);
	virtual ~UPrimitiveComponent() {}

	// 별도의 초기화 메서드
	virtual bool Init();

	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return mesh; }

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { return Color; }
};
