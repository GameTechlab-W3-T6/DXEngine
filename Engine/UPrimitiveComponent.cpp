#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
bool UPrimitiveComponent::Init(UMeshManager* meshManager)
{
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
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);
}

void FTexture::LoadTexture(const std::string& path)
{
	//DirectX::CreateDDSTextureFromFile();
	
	// Load 
	D3D11_TEXTURE2D_DESC td = {};

}
