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

bool UPrimitiveComponent::Init(UMeshManager* meshManager, UInputManager* in, UTextureManager* tm, UCamera* cam)
{
	inputManager = in;
	textureManager = tm;
	/*
	if (textureManager && meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		texture = textureManager->RetrieveTexture(GetClass()->GetMeta("TextureType")); 
		
		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo")); 
		textInfo->SetParam(textTex, 16, 16); 
		camera = cam;

		return mesh != nullptr;
	}*/

	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		return mesh != nullptr;
	}
	 

	return false;
}

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected);

	if (textInfo->textTexture != nullptr)
	{
		renderer.SetTextUV(*textInfo);
	} 
} 

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(renderer);

	renderer.DrawMesh(mesh);
}