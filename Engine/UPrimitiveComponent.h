#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Shader.h"
#include "USceneComponent.h"
#include "Vector.h"
#include "UClass.h"
#include "ConfigData.h"
#include "ConfigManager.h"

#include "FTextInfo.h"

class UMeshManager; // 전방 선언
class UTextureManager;
//class UCamera;
class FTexture;
class UTextholderComp;

/**
 * @brief Renderable component with mesh and material properties
 */
class UPrimitiveComponent : public USceneComponent
{
	DECLARE_UCLASS(UPrimitiveComponent, USceneComponent)
public:
	using LayerID = uint32;
protected:
	UMesh* mesh;
	FTexture* texture;
	UShader* vertexShader, *pixelShader;
	FVector4 Color = { 1, 1, 1, 1 };
	bool cachedIsShaderReflectionEnabled;
	bool bAutoCreateTextholder;
public:
	UPrimitiveComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr), vertexShader(nullptr),  pixelShader(nullptr), bAutoCreateTextholder(true)
	{
		ConfigData* config = ConfigManager::GetConfig("editor");
		cachedIsShaderReflectionEnabled = config->getBool("Graphics", "ShaderReflection");

		name = FName(GetDefaultName());
		ID = PrimitiveID++;
	}

	bool bIsVisible = true;

	virtual void UpdateConstantBuffer(URenderer& renderer);

	virtual void BindVertexShader(URenderer& renderer);

	virtual void BindPixelShader(URenderer& renderer);

	void BindShader(URenderer& renderer);

	void BindMesh(URenderer& renderer);

	void BindTexture(URenderer& renderer);

	bool Initialize() override;  // TODO: Rename to InitializeComponent() later
	virtual void Update(float deltaTime) override;  // TODO: Rename to TickComponent() later
	virtual void OnShutdown() override;  // TODO: Rename to EndPlay() later

	virtual void Draw(URenderer& renderer);

	virtual LayerID GetLayer() const { return 2;  }

	virtual ~UPrimitiveComponent() {}

	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return mesh; }

	UShader* GetVertexShader() { return vertexShader;  }

	UShader* GetPixelShader() { return pixelShader;  }

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { return Color; }

public:
	virtual uint32 GetID() const { return ID;  }

protected:
	virtual const char* GetDefaultName() const { return "Primitive"; }

private:
	static uint32 PrimitiveID;
	uint32 ID;
};
