#include "stdafx.h"

#include "UBatchRenderer.h"
#include "UClass.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"

IMPLEMENT_UCLASS(UBatchRenderer, URenderer)

void UBatchRenderer::DrawPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent)
{
	URenderer::DrawPrimitiveComponent(PrimitiveComponent);
}

void UBatchRenderer::SwapBuffer()
{
	Draw();

	URenderer::SwapBuffer();
}

void UBatchRenderer::Draw()
{
	// URenderer::Draw();

	UE_LOG("Draw!");
}
