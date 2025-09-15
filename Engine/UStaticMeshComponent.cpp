#include "stdafx.h"
#include "UStaticMeshComponent.h"
#include "UMeshManager.h"
#include "UTextureManager.h"

IMPLEMENT_UCLASS(UStaticMeshComponent, UPrimitiveComponent)
UCLASS_META(UStaticMeshComponent, TextInfo, "TextInfo");
void UStaticMeshComponent::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	if (!initialized)
	{
		InitializeUUID();
		initialized = true;
	}

	// draw quad too!
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled);
	renderer.DrawMesh(mesh);

	FVector ParentLocation = GetPosition();
	FVector Offset = FVector(30.0f, 30.0f, 30.0f); // 월드 z축 + 5정도 되는 위치 = 머리 위
	FVector labelLocation = ParentLocation + Offset;
	uuidQuad.DrawAboveParent(labelLocation);
}