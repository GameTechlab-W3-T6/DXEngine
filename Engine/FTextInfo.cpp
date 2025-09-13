#include "stdafx.h"
#include "FTextInfo.h" 

void FTextInfo::SetParam(FTexture* textTex, int32 PerRow, int32 PerCol)
{
	textTexture = textTex;
	cellsPerColumn = 16;
	cellsPerRow = 16;
	cellWidth = textTex->width/ cellsPerRow;
	cellHeight = textTex->height / cellsPerColumn;
}
