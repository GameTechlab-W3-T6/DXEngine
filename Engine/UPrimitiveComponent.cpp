#include "stdafx.h"
#include "ConfigManager.h"
#include "FTextInfo.h"
#include "FTexture.h"
#include "UBatchShaderManager.h"
#include "UInputManager.h"
#include "UMeshManager.h"
#include "UPrimitiveComponent.h"
#include "URenderer.h"
#include "UTextureManager.h"
#include "USceneManager.h"
#include "USceneComponent.h"
#include "UScene.h"
#include "UTextholderComp.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)

uint32 UPrimitiveComponent::PrimitiveID = 0;

bool UPrimitiveComponent::Initialize()
{
	if (!USceneComponent::Initialize()) return false;
	UMeshManager* meshManager = UEngineStatics::GetSubsystem<UMeshManager>();
	UBatchShaderManager* batchShaderManager = UEngineStatics::GetSubsystem<UBatchShaderManager>();
	UTextureManager* textureManager = UEngineStatics::GetSubsystem<UTextureManager>();
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

	texture = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));

	// Auto-create and attach textholder component
	if (bAutoCreateTextholder)
	{
		UTextholderComp* textholderComp = new UTextholderComp();
		textholderComp->SetParentTransform(this);
		textholderComp->Initialize();

		// Set UUID text for the primitive component
		const FString uuidText = "UID : " + std::to_string(UUID);
		textholderComp->SetText(uuidText);

		AttachChild(textholderComp);
	}

	return mesh && vertexShader && pixelShader;
}

void UPrimitiveComponent::Update(float deltaTime)
{
	// Call parent update first
	Super::Update(deltaTime);

	// Add primitive-specific update logic here if needed
}

void UPrimitiveComponent::OnShutdown()
{
	// Cleanup primitive-specific resources
	// Note: mesh, shaders, texture are managed by subsystems, don't delete them here

	// Parent class will handle cleanup of all attached children including textholder
	Super::OnShutdown();
}

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix MVP = GetWorldTransform() * renderer.GetViewProj();
	(*vertexShader)["ConstantBuffer"]["MVP"] = MVP;
	(*vertexShader)["ConstantBuffer"]["MeshColor"] = Color;
	(*vertexShader)["ConstantBuffer"]["IsSelected"] = bIsSelected;
	vertexShader->BindConstantBuffer(renderer.GetDeviceContext(), "ConstantBuffer");
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

void UPrimitiveComponent::BindTexture(URenderer& renderer)
{
	/** @todo: Hard-coded slot number. */
	// texture를 보내주는ㄱ ㅔ맞을ㅇ듯Bind 
	texture->Bind(renderer.GetDeviceContext(), 0);
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer || !vertexShader || !pixelShader)
	{
		return;
	}

	renderer.DrawPrimitiveComponent(this);

	// Draw attached child components
	for (USceneComponent* child : AttachChildren)
	{
		if (UPrimitiveComponent* childPrimitive = child->Cast<UPrimitiveComponent>())
		{
			childPrimitive->Draw(renderer);
		}
	}
}