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
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected);
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer || !vertexShader || !pixelShader)
	{
		return;
	}

	renderer.SetShader(vertexShader, pixelShader);
	UpdateConstantBuffer(renderer);
	renderer.DrawPrimitiveComponent(this);
	//renderer.DrawMesh(mesh);
}