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
	
	inline void Build3x4Rows(const FMatrix& M, float outM0[4], float outM1[4], float outM2[4])
	{
		// 너의 FMatrix가 row-major라고 했으니 행 기준으로 꺼내면 됨
		// 행 0,1,2의 xyz, 그리고 마지막 행(3)의 x/y/z를 각 .w로
		outM0[0] = M.M[0][0]; outM0[1] = M.M[0][1]; outM0[2] = M.M[0][2]; outM0[3] = M.M[3][0];
		outM1[0] = M.M[1][0]; outM1[1] = M.M[1][1]; outM1[2] = M.M[1][2]; outM1[3] = M.M[3][1];
		outM2[0] = M.M[2][0]; outM2[1] = M.M[2][1]; outM2[2] = M.M[2][2]; outM2[3] = M.M[3][2];

	}

	inline void ComputeGlyphUVRect(
		int code, int cellsPerRow, int cellsPerCol,
		int cellW, int cellH, int texW, int texH,
		float outUVOffset[2], float outUVScale[2])
	{
		const int col = code % cellsPerRow;
		const int row = code / cellsPerRow;

		const float du = (float)cellW / (float)texW;
		const float dv = (float)cellH / (float)texH;
		const float u0 = (float)(col * cellW) / (float)texW;
		const float v0 = (float)(row * cellH) / (float)texH;

		outUVOffset[0] = u0; outUVOffset[1] = v0;
		outUVScale[0] = du; outUVScale[1] = dv;
	}
	virtual void UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
	virtual void Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled) override;
};

