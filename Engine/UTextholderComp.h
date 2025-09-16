#pragma once
#include "stdafx.h"
#include "UObjectMacros.h"
#include "UPrimitiveComponent.h"

struct FTextInstance
{
	float M0[4];
	float M1[4];
	float M2[4];

	float uvOffset[2];
	float uvScale[2];

	float color[4];
};

class USceneManager;
class UTextureManager;
class UTextholderComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UTextholderComp, UPrimitiveComponent)
public:
	virtual ~UTextholderComp() = default;

	UTextholderComp()
	{
		Name = GetDefaultName();
		ID = TextHolderID++;
	}

	bool Initialize() override;
	void SetText(const FString& textContent);
	void SetParentTransform(USceneComponent* parent) { parentTransform = parent; }

	// ============================= //

	const TArray<FTextInstance>& GetInstance() const
	{
		return instances;
	}

	// ============================= //

	virtual void UpdateConstantBuffer(URenderer& renderer) override;

	virtual void BindVertexShader(URenderer& renderer) override;

	virtual void BindPixelShader(URenderer& renderer) override;

	virtual void Draw(URenderer& renderer) override;

	virtual LayerID GetLayer() const { return 4;  }

	// todo : 추후 textholder도 gizmo 필요하면 추가 구현
	bool IsManageable() override { return false; }

private:
	// Hold those two subsystem due to caching
	UTextureManager* cachedTextureManager;
	UInputManager* cachedInputManager;
	USceneComponent* parentTransform;
	//  TODO : pointer로 들고 있기
	FTextInfo TextInfo;

	// ============================= //

	TArray<FTextInstance> instances;

	// ============================= //

	bool isEditable = true;
	
	void CaptureTypedChars();                 // 이번 프레임 타이핑된 글자들을 수집

	void CreateInstanceData();

	inline void Build3x4Rows(const FMatrix& M, float outM0[4], float outM1[4], float outM2[4])
	{
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

	virtual uint32 GetID() const { return ID;  }

protected:
	virtual const char* GetDefaultName() const override { return "Textholder"; }

private:
	static uint32 TextHolderID;
	uint32 ID;
};
