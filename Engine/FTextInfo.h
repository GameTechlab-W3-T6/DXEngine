#pragma once
#include "FTexture.h"
#include "UEngineStatics.h"
 
struct FMatrix;
struct FTextInfo
{ 
	std::vector<int> orderOfChar;
	
	int keyCode = 0;
	float center;
	FTexture* textTexture; 
	int cellIndex[2];
	   
	float cellsPerRow, cellsPerColumn; 
	float cellWidth, cellHeight;   
	float size[2] = { 1.0f ,1.0f };

	void SetParam(FTexture* textTex, int32 PerRow, int32 PerCol);
	FMatrix MakeBillboard(FMatrix view);
};
