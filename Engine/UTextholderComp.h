#pragma once
#include "stdafx.h"
#include "UPrimitiveComponent.h"

class USceneManager;
class UTextureManager;
class UTextholderComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UTextholderComp, UPrimitiveComponent)
public:
	void Initialize();
	void SetText(const FString& textContent);
	void SetText(int32 InNumber);
	void DrawAboveParent(FVector location);

private:
	//  TODO : pointer로 들고 있기
	FTextInfo TextInfo;
	bool isEditable = true;
	bool isInitailized = false;

	void CaptureTypedChars();                 // 이번 프레임 타이핑된 글자들을 수집
	void RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled); // 수집된 글자를 가로로 나열 렌더

	virtual void UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
};

