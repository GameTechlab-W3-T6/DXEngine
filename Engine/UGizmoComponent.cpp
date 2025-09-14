#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UClass.h"
#include "FTextInfo.h" 

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

void UGizmoComponent::UpdateConstantBuffer(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, GetColor(), bIsSelected, bIsShaderReflectionEnabled);
	renderer.SetTextUV(std::nullopt, false, true);
}

void UGizmoComponent::Update(float deltaTime)
{
}

void UGizmoComponent::Draw(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer, bIsShaderReflectionEnabled);
	renderer.DrawMesh(mesh);
}

void UGizmoComponent::DrawOnTop(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer, bIsShaderReflectionEnabled);
	renderer.DrawMeshOnTop(mesh);
}