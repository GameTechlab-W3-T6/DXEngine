#include "stdafx.h"
#include "FTextInfo.h" 
#include "Matrix.h"
#include "Vector.h"

void FTextInfo::SetParam(FTexture* textTex, int32 PerRow, int32 PerCol)
{
    textTexture = textTex;
    cellsPerColumn = 16;
    cellsPerRow = 16;
    cellWidth = textTex->width / cellsPerRow;
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

FMatrix FTextInfo::MakeBillboard(FVector objPos, FVector camPos)
{
    FVector forward = (camPos - objPos).Normalized();

    FVector up = FVector(0, 0, 1); // 월드 Z-up 강제
    FVector right = up.Cross(forward).Normalized();
    up = forward.Cross(right).Normalized();

    FMatrix billboard;
    billboard.M[0][0] = right.X;   billboard.M[0][1] = right.Y;   billboard.M[0][2] = right.Z;   billboard.M[0][3] = 0;
    billboard.M[1][0] = up.X;      billboard.M[1][1] = up.Y;      billboard.M[1][2] = up.Z;      billboard.M[1][3] = 0;
    billboard.M[2][0] = forward.X; billboard.M[2][1] = forward.Y; billboard.M[2][2] = forward.Z; billboard.M[2][3] = 0;
    billboard.M[3][0] = 0;         billboard.M[3][1] = 0;         billboard.M[3][2] = 0;         billboard.M[3][3] = 1;

    return billboard;
}
