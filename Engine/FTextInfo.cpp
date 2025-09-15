#include "stdafx.h"
#include "FTextInfo.h" 
#include "Matrix.h"

void FTextInfo::SetParam(FTexture* textTex, int32 PerRow, int32 PerCol)
{
	textTexture = textTex;
	cellsPerColumn = 16;
	cellsPerRow = 16;
	cellWidth = textTex->width/ cellsPerRow;
	cellHeight = textTex->height / cellsPerColumn;
}

FMatrix FTextInfo::MakeBillboard(FMatrix view)
{
	bool ok = false;
	FMatrix invV = FMatrix::Inverse(view, &ok);
    if (!ok) return FMatrix::IdentityMatrix();

    invV.M[3][0] = invV.M[3][1] = invV.M[3][2] = 0.0f; ///< translation 제거
 
    return invV; 

}
