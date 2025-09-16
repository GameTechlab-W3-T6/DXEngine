#include "stdafx.h"
#include "FTextInfo.h" 
#include "Matrix.h"
#include "Vector.h"

void FTextInfo::SetParam(FTexture* textTex, int32 PerRow, int32 PerCol)
{
    textTexture = textTex;
    cellsPerColumn = PerCol;
    cellsPerRow = PerRow;
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
    // LH Z-up 좌표계에서 billboard 계산
    FVector toCamera = (camPos - objPos).Normalized();

    // LH Z-up에서 world up은 +Z
    FVector worldUp = FVector(0, 0, 1);

    // LH 좌표계에서 올바른 cross product 순서
    // right = forward x up (LH에서)
    FVector right = toCamera.Cross(worldUp).Normalized();

    // 만약 카메라가 정확히 위나 아래에 있다면 다른 벡터 사용
    if (right.Length() < 0.001f) {
        right = FVector(1, 0, 0); // X축을 right으로 사용
    }

    // up = right x forward (LH에서)
    FVector up = right.Cross(toCamera).Normalized();

    // Row-major 매트릭스에서 행 벡터로 구성
    FMatrix billboard;
    billboard.M[0][0] = right.X;    billboard.M[0][1] = right.Y;    billboard.M[0][2] = right.Z;    billboard.M[0][3] = 0;
    billboard.M[1][0] = up.X;       billboard.M[1][1] = up.Y;       billboard.M[1][2] = up.Z;       billboard.M[1][3] = 0;
    billboard.M[2][0] = toCamera.X; billboard.M[2][1] = toCamera.Y; billboard.M[2][2] = toCamera.Z; billboard.M[2][3] = 0;
    billboard.M[3][0] = 0;          billboard.M[3][1] = 0;          billboard.M[3][2] = 0;          billboard.M[3][3] = 1;

    return billboard;
}
