#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UTextureManager.h"
#include "FTextInfo.h"
#include "FTexture.h"
#include "UInputManager.h"
#include "UCamera.h"

//#include <fstream>
//#include <sstream>

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
UCLASS_META(UPrimitiveComponent, TextInfo, "TextInfo");

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
	renderer.SetTextUV(*textInfo, bUseTextTexture, bIsShaderReflectionEnabled);
	
}

bool UPrimitiveComponent::Init(URenderer* rd, UMeshManager* mM, UInputManager* im, UTextureManager* tm, UCamera* cam)
{
	renderer = rd;
	inputManager = im;
	textureManager = tm;
	meshManager = mM;

	if (!meshManager)
		return false;

	mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));

	if (textureManager)
	{
		texture = textureManager->RetrieveTexture(GetClass()->GetMeta("TextureType"));

		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		if (textTex)
		{
			textInfo->SetParam(textTex, 16, 16);
			camera = cam;
		}
	}

	return mesh != nullptr;
}

void UPrimitiveComponent::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled);
	renderer.DrawMesh(mesh);
}