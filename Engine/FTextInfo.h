#pragma once
#include "FTexture.h"

struct FTextInfo
{
	std::vector<int> orderOfChar;

	FTexture* textTexture; 
	  
	int keyCode;
	float cellsPerRow, cellsPerColumn; 
	float cellWidth, cellHeight; 
	float u, v;

	float space = 1.0f; 

	void CallCharacter(int keyCode);
};
