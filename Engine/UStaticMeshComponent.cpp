#include "stdafx.h"
#include "UStaticMeshComponent.h"
#include "UMeshManager.h"
#include "UTextureManager.h"

IMPLEMENT_UCLASS(UStaticMeshComponent, UPrimitiveComponent)
UCLASS_META(UStaticMeshComponent, TextInfo, "TextInfo");
void UStaticMeshComponent::Draw(URenderer& renderer)
{
	UPrimitiveComponent::Draw(renderer);
	if (!initialized)
	{
		InitializeUUID();
		initialized = true;
	}

	FVector ParentLocation = GetPosition();
	FVector Offset = FVector(0.0f, 0.0f, 0.5f); // 월드 z축 + 5정도 되는 위치 = 머리 위
	FVector labelLocation = ParentLocation + Offset;
	// uuidQuad.SetPosition(ParentLocation);
	uuidQuad.DrawAboveParent(renderer, labelLocation);
}