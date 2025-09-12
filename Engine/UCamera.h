#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
// =====================
// UCamera (LH, row-vector, Z-up)
// =====================


class UCamera
{
public:
    UCamera()
        : mEye(0.0f, 0.0f, 0.0f)
        , mRot(FQuaternion::Identity())
        , mView(FMatrix::IdentityMatrix())
        , mProj(FMatrix::IdentityMatrix())
        , mFovY(ToRad(60.0f)), mAspect(1.0f), mNearZ(0.1f), mFarZ(1000.0f) 
        , mUseOrtho(false)
        , mOrthoWidth(10.0f)
        , mOrthoHeight(10.0f)
        , bLockRoll(false)
        , mPitch(0.0f)
    {
        RecalcAxesFromQuat();
        UpdateView();
        UpdateProj();
    }
    // ===== 행렬 Get =====

    const FMatrix& GetView() const { return mView; }
    const FMatrix& GetProj() const { return mProj; }


    /// 투영
	// ===== 투영 파라미터 Get =====

    float GetAspect() const { return mAspect; }
    float GetNearZ() const { return mNearZ; }
    float GetFarZ()  const { return mFarZ; }


    // ===== 직교 투영 여부 Get =====

    bool  IsOrtho()  const { return mUseOrtho; }


    // ===== 투영 Set =====

    void SetPerspective(float fovY, float aspect, float zn, float zf);
    void SetPerspectiveDegrees(float fovYDeg, float aspect, float zn, float zf);
    float GetFOV() const { return mFovY * (180.0f / PI); }
    void  SetFOV(float deg) { SetPerspectiveDegrees(deg, mAspect, mNearZ, mFarZ); }
    // w,h는 뷰 공간 가로/세로 범위(직교 박스 크기)
    void SetOrtho(float w, float h, float zn, float zf, bool leftHanded = false);
    void SetAspect(float aspect) { mAspect = (aspect > 0.0f) ? aspect : 1.0f; UpdateProj(); }


	// ===== 위치/자세 Get =====

    const FVector& GetLocation() const { return mEye; }
    const FQuaternion& GetRotation()   const { return mRot; }
    const FVector& GetRight()   const { return mRight; }
    const FVector& GetUp()      const { return mUp; }
    const FVector& GetForward() const { return mForward; }
    void GetBasis(FVector& outRight, FVector& outForward, FVector& outUp) const;


    // ===== 위치/자세 Set =====

    void SetLocation(const FVector& eye) { mEye = eye; UpdateView(); }
    void SetRotation(const FQuaternion& q) { mRot = q.Normalized(); UpdateView(); }
    void SetLockRoll(bool on) { bLockRoll = on; UpdateView(); }
	// 카메라가 특정 지점을 바라보도록 설정 (LH, Z-up)
    void LookAt(const FVector& eye, const FVector& target, const FVector& up = FVector(0, 0, 1));
    // 로컬축 기준 이동 (dx=right, dy=forward, dz=up)
    void MoveLocal(float dx, float dy, float dz, float deltaTime, bool boost = false,
        float baseSpeed = 3.0f, float boostMul = 3.0f);


	// ==== Yaw/Pitch 제어 ====

    // 무제한 마우스룩(쿼터니언): yaw=world Z, pitch=야우 적용 후의 Right
    void AddYawPitch(float yawZ, float pitch);
    void GetYawPitch(float& yawZ, float& pitch) const;
    void SetYawPitch(float yawZ, float pitch);


	// ==== 카메라 Rotation 기반 오일러 얻기/설정 (XYZ 순서, 월드 기준) ====

	// 절대값 얻기: 월드 기준 Euler(X→Y→Z)로 '얻기'
    FVector GetEulerXYZRad() const;
    FVector GetEulerXYZDeg() const;
    // 절대값 설정: 월드 기준 Euler(X→Y→Z)로 '설정'
    // - row-vector 규약에서 ToMatrixRow(qx*qy*qz) == Rx * Ry * Rz
    // - 즉 X, Y, Z 순서로 월드 회전을 적용한 결과를 그대로 mRot에 대입
    void SetEulerXYZRad(float rx, float ry, float rz);
    void SetEulerXYZDeg(float rxDeg, float ryDeg, float rzDeg);

private:
    // ---- 내부 상태 ----
    FVector mEye;      // 월드 위치
    FQuaternion mRot;  // 자세(orientation)

    // 파생/캐시 축 (뷰/이동에 사용)
    FVector mRight;
    FVector mUp;
    FVector mForward;

    // 투영 파라미터
    float   mFovY;
    float   mAspect;
    float   mNearZ, mFarZ;
    bool    mUseOrtho;
    float   mOrthoWidth, mOrthoHeight;

    // 누적 pitch
    float mPitch;

    // 행렬
    FMatrix mView;
    FMatrix mProj;
	// 롤 잠금
    bool  bLockRoll;

    // 카메라 정보 갱신

    // mRot로부터 mRight, mUp, mForward 갱신
    void RecalcAxesFromQuat();
    void UpdateProj(bool leftHanded = true);
    void UpdateView();
};
