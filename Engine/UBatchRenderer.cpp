#include "stdafx.h"

#include "UBatchRenderer.h"
#include "UClass.h"
#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

IMPLEMENT_UCLASS(UBatchRenderer, URenderer)

void UBatchRenderer::DrawPrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	URenderer::DrawPrimitive(PrimitiveComponent);
}

void UBatchRenderer::SwapBuffer()
{
	URenderer::SwapBuffer();
}

void UBatchRenderer::Draw()
{
	//URenderer::Draw();
}
