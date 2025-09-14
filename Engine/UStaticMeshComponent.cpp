#include "stdafx.h"
#include "UStaticMeshComponent.h"
#include "UMeshManager.h"

IMPLEMENT_UCLASS(UStaticMeshComponent, UPrimitiveComponent)
void UStaticMeshComponent::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	// draw quad too!
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled);
	renderer.DrawMesh(mesh);

	FVector labelLocation = GetPosition() + (0, 0, 5); // 월드 z축 + 5정도 되는 위치 = 머리 위
	uuidQuad.Draw(labelLocation);
}