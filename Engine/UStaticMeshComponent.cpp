#include "stdafx.h"
#include "UStaticMeshComponent.h"
#include "UMeshManager.h"
#include "UTextureManager.h"

IMPLEMENT_UCLASS(UStaticMeshComponent, UPrimitiveComponent)
UCLASS_META(UStaticMeshComponent, TextInfo, "TextInfo");
void UStaticMeshComponent::Draw(URenderer& renderer)
{
	if (!initialized)
	{
		InitializeUUID();
		initialized = true;
	}

	// draw quad too!
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);

	FVector ParentLocation = GetPosition();
	FVector Offset = FVector(0.0f, 0.0f, 0.25f); // 월드 z축 + 5정도 되는 위치 = 머리 위
	FVector labelLocation = ParentLocation + Offset;
	// uuidQuad.SetPosition(ParentLocation);
	uuidQuad.DrawAboveParent(renderer, labelLocation);
}