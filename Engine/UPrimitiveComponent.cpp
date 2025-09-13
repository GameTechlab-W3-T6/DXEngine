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

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
}

void UPrimitiveComponent::Draw(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer, bIsShaderReflectionEnabled);
	renderer.DrawMesh(mesh);
}