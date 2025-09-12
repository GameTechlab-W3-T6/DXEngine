#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UGizmoComponent, USceneComponent)

bool UGizmoComponent::Init(UMeshManager* meshManager)
{
	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		return mesh != nullptr;
	}
	return false;
}

FMatrix UGizmoComponent::GetWorldTransform()
{
	return FMatrix::SRTRowQuaternion(RelativeLocation, (OriginQuaternion * RelativeQuaternion).ToMatrixRow(), RelativeScale3D);
}

void UGizmoComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, GetColor(), bIsSelected); 
}

void UGizmoComponent::Update(float deltaTime)
{
}

void UGizmoComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer); 
	renderer.DrawMesh(mesh);
}

void UGizmoComponent::DrawOnTop(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMeshOnTop(mesh);
}