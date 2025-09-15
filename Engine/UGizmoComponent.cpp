#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UClass.h"
#include "UBatchShaderManager.h"
#include "FTextInfo.h" 

IMPLEMENT_UCLASS(UGizmoComponent, UPrimitiveComponent)

bool UGizmoComponent::Init()
{
	UMeshManager* meshManager = UEngineStatics::GetSubsystem<UMeshManager>();
	UBatchShaderManager* batchShaderManager = UEngineStatics::GetSubsystem<UBatchShaderManager>();
	if (meshManager)
	{
		mesh = meshManager->GetMesh(GetClass()->GetMeta("MeshName"));
	}
	if (batchShaderManager)
	{
		std::string vertexShaderName = GetClass()->GetMeta("VertexShaderName");
		std::string pixelShaderName = GetClass()->GetMeta("PixelShaderName");

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
	UPrimitiveComponent::BindVertexShader(renderer);
}

void UGizmoComponent::DrawOnTop(URenderer& renderer)
{
	// TODO : delete this
	// renderer.SetShader(vertexShader, pixelShader);
	// UpdateConstantBuffer(renderer);
	renderer.DrawGizmoComponent(this, true);
}