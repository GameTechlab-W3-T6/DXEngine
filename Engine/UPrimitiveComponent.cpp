#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "ConfigManager.h"
#include "UBatchShaderManager.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
bool UPrimitiveComponent::Init()
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

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix MVP = GetWorldTransform() * renderer.GetViewProj();
	(*vertexShader)["ConstantBuffer"]["MVP"] = MVP;
	(*vertexShader)["ConstantBuffer"]["MeshColor"] = Color;
	(*vertexShader)["ConstantBuffer"]["IsSelected"] = bIsSelected;
}

void UPrimitiveComponent::BindVertexShader(URenderer& renderer)
{
	vertexShader->Bind(renderer.GetDeviceContext(), "ConstantBuffer");
}

void UPrimitiveComponent::BindPixelShader(URenderer& renderer)
{
	pixelShader->Bind(renderer.GetDeviceContext());
}

void UPrimitiveComponent::BindShader(URenderer& renderer)
{
	BindVertexShader(renderer);
	BindPixelShader(renderer);
}

void UPrimitiveComponent::BindMesh(URenderer& renderer)
{
	mesh->Bind(renderer.GetDeviceContext());
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer || !vertexShader || !pixelShader)
	{
		return;
	}

	renderer.DrawPrimitiveComponent(this);
}