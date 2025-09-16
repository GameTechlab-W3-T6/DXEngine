#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UClass.h"
#include "UBatchShaderManager.h"

IMPLEMENT_UCLASS(UGizmoComponent, UPrimitiveComponent)

bool UGizmoComponent::Initialize()
{
	UMeshManager* meshManager = UEngineStatics::GetSubsystem<UMeshManager>();
	UBatchShaderManager* batchShaderManager = UEngineStatics::GetSubsystem<UBatchShaderManager>();
	if (meshManager)
	{
		mesh = meshManager->GetMesh(GetClass()->GetMeta("MeshName"));
	}
	if (batchShaderManager)
	{
		FString vertexShaderName = GetClass()->GetMeta("VertexShaderName");
		FString pixelShaderName = GetClass()->GetMeta("PixelShaderName");

		if (vertexShaderName == "")
			vertexShaderName = "Vertex";
		if (pixelShaderName == "")
			pixelShaderName = "Pixel";

		vertexShader = batchShaderManager->GetShaderByName(vertexShaderName);
		pixelShader = batchShaderManager->GetShaderByName(pixelShaderName);
	}

	return mesh && vertexShader && pixelShader;
}

FMatrix UGizmoComponent::GetWorldTransform()
{
	return FMatrix::SRTRowQuaternion(RelativeLocation, (OriginQuaternion * RelativeQuaternion).ToMatrixRow(), RelativeScale3D);
}

void UGizmoComponent::Update(float deltaTime)
{
}

void UGizmoComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix MVP = GetWorldTransform() * renderer.GetViewProj();
	(*vertexShader)["ConstantBuffer"]["MVP"] = MVP;
	(*vertexShader)["ConstantBuffer"]["MeshColor"] = Color;
	(*vertexShader)["ConstantBuffer"]["IsSelected"] = bIsSelected;
	vertexShader->BindConstantBuffer(renderer.GetDeviceContext(), "ConstantBuffer");
}

void UGizmoComponent::BindVertexShader(URenderer& renderer)
{
	UPrimitiveComponent::BindVertexShader(renderer);
}

void UGizmoComponent::BindPixelShader(URenderer& renderer)
{
	UPrimitiveComponent::BindPixelShader(renderer);
}

void UGizmoComponent::Draw(URenderer& renderer)
{
	renderer.DrawGizmoComponent(this);
}

void UGizmoComponent::DrawOnTop(URenderer& renderer)
{
	renderer.DrawGizmoComponent(this, true);
}