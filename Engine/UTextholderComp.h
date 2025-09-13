#pragma once
#include "stdafx.h"
#include "UPrimitiveComponent.h"

class USceneManager;
class UTextholderComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UTextholderComp, UPrimitiveComponent)

	bool Initialize(UCamera* InCamera, USceneManager* InSceneManager);

	void CaptureTypedChars();                 // 이번 프레임 타이핑된 글자들을 수집
	void RenderTextLine(URenderer& renderer); // 수집된 글자를 가로로 나열 렌더
	
	virtual bool Init(UMeshManager* meshManager, UInputManager* im, UTextureManager* tm = nullptr, UCamera* cam = nullptr) override;
	virtual void Draw(URenderer& renderer) override;
};

