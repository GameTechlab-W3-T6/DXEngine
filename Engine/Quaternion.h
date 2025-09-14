#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Vector4.h"
#include "Matrix.h"
#include <math.h>


inline float ToRad(float d) { return d * (float)(PI / 180.0); }
inline float ToDeg(float r) { return r * (180.0f / (float)PI); }
inline FVector ToRad(FVector v) { return { ToRad(v.X), ToRad(v.Y), ToRad(v.Z) }; }
inline FVector ToDeg(FVector v) { return { ToDeg(v.X), ToDeg(v.Y), ToDeg(v.Z) }; }
/**
 * @brief Quaternion structure for 3D rotations with SLERP support
 */
struct FQuaternion
{
	// (x,y,z,w) with w = scalar
	// 즉, 쿼터니언은 W가 실수부이고 x, y, z는 허수부이다.
	float X, Y, Z, W;

	FQuaternion(float x = 0, float y = 0, float z = 0, float w = 1) : X(x), Y(y), Z(z), W(w) {}

	static FQuaternion Identity() { return FQuaternion(0, 0, 0, 1); }

	// 기본 연산
	float Dot(const FQuaternion& o) const { return X * o.X + Y * o.Y + Z * o.Z + W * o.W; }

	float Length() const { return sqrtf(X * X + Y * Y + Z * Z + W * W); }

	FQuaternion Normalized() const
	{
		float l = Length();
		if (l <= 0.0f) return Identity();
		float inv = 1.0f / l;
		return FQuaternion(X * inv, Y * inv, Z * inv, W * inv);
	}
	// 켤레
	FQuaternion Conjugate() const { return FQuaternion(-X, -Y, -Z, W); }

	// 단위쿼터니언이면 Inverse = conjugate
	// Inverse는 비단위도 계산가능
	FQuaternion Inverse() const
	{
		// 일반형: conj / |q|^2
		float n2 = X * X + Y * Y + Z * Z + W * W;
		if (n2 <= 0.0f) return Identity();
		float inv = 1.0f / n2;
		return FQuaternion(-X * inv, -Y * inv, -Z * inv, W * inv);
	}

	/*
		표준 해밀턴 곱(H): "먼저 a, 그 다음 b"를 column 규약으로 표현하면 b ⊗ a.
		우리는 row 규약이므로 "행렬 곱 순서와 동일"하도록 정의:
		ToMatrixRow(a * b) == ToMatrixRow(a) * ToMatrixRow(b)
		→ operator*는 H(b,a)로 구현한다.
	*/
	static FQuaternion Hamilton(const FQuaternion& p, const FQuaternion& q)
	{
		// 표준 해밀턴 곱: q ⊗ p (column 규약: 먼저 p, 그 다음 q)
		return FQuaternion(
			q.W * p.X + q.X * p.W + q.Y * p.Z - q.Z * p.Y,
			q.W * p.Y - q.X * p.Z + q.Y * p.W + q.Z * p.X,
			q.W * p.Z + q.X * p.Y - q.Y * p.X + q.Z * p.W,
			q.W * p.W - q.X * p.X - q.Y * p.Y - q.Z * p.Z
		);
	}
	// 사원수 곱셈은 교환법칙 성립 X 유의해서 사용
	// 먼저 this, 그 다음 quat
	FQuaternion operator*(const FQuaternion& quat) const
	{
		// row 규약 합성: 먼저 this, 그 다음 quat
		// → 표준 해밀턴으로는 Hamilton(this, quat)가 아니라 Hamilton(quat, this)
		// *this ⊗ quat
		return Hamilton(quat, *this);
	}
	/*
		스칼라 보간/SLERP
		계산 빠름 (성분끼리 그냥 선형 보간)
		근사적인 회전 → 두 회전 사이가 "대략적"으로 부드럽게 이어짐
		짧은 각도 차이에서는 SLERP와 거의 차이가 없음
	*/
	static FQuaternion Lerp(const FQuaternion& a, const FQuaternion& b, float t)
	{
		FQuaternion r(
			a.X + (b.X - a.X) * t,
			a.Y + (b.Y - a.Y) * t,
			a.Z + (b.Z - a.Z) * t,
			a.W + (b.W - a.W) * t
		);
		return r.Normalized();
	}
	/*
		두 쿼터니온이 만드는 4차원 단위구면(S³) 위의 "최단 호"를 따라 회전
		회전 속도가 일정해 보임(등속 회전)
		정확하지만 계산이 조금 더 무거움(삼각함수 사용)
		쿼터니온은 단위 4차원 벡터 (S³ 구면 위의 점)
		두 쿼터니온 a, b는 같은 구면 위의 점
		따라서 "구 위의 최단 경로(호)"를 따라가며 보간하는 게 SLERP
		즉, a에서 b로 가는 대원(geodesic) 을 따라t만큼 이동하는 원리
	*/
	static FQuaternion Slerp(FQuaternion a, FQuaternion b, float t)
	{
		float cosv = a.Dot(b);
		if (cosv < 0.0f) { b = FQuaternion(-b.X, -b.Y, -b.Z, -b.W); cosv = -cosv; }
		const float EPS = 1e-6f;
		if (cosv > 1.0f - EPS) return Lerp(a, b, t);
		float ang = acosf(cosv);
		float sA = sinf((1.0f - t) * ang);
		float sB = sinf(t * ang);
		float invS = 1.0f / sinf(ang);
		FQuaternion r(
			(a.X * sA + b.X * sB) * invS,
			(a.Y * sA + b.Y * sB) * invS,
			(a.Z * sA + b.Z * sB) * invS,
			(a.W * sA + b.W * sB) * invS
		);
		return r.Normalized();
	}

	// 생성기
	static FQuaternion FromAxisAngle(FVector axis, float rad)
	{
		float len = sqrtf(axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z);
		if (len <= 0.0f) return Identity();
		float inv = 1.0f / len; axis.X *= inv; axis.Y *= inv; axis.Z *= inv;
		float s = sinf(rad * 0.5f), c = cosf(rad * 0.5f);
		return FQuaternion(axis.X * s, axis.Y * s, axis.Z * s, c).Normalized();
	}

	// 행벡터 규약: X→Y→Z 순서라면 실제 행렬은 S*Rx*Ry*Rz 같은 식으로 오른쪽에 쌓임.
	static FQuaternion FromEulerXYZ(float rx, float ry, float rz)
	{
		FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), rx);
		FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), ry);
		FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), rz);
		// 먼저 X, 그 다음 Y, 그 다음 Z → row 규약 합성
		return qx * qy * qz;
	}
	static FQuaternion FromEulerXYZDeg(float degX, float degY, float degZ)
	{
		return FromEulerXYZ(ToRad(degX), ToRad(degY), ToRad(degZ)).Normalized();
	}
	static FQuaternion FromEulerXYZDeg(FVector v)
	{
		return FromEulerXYZDeg(v.X, v.Y, v.Z);
	}
	// forward 방향으로 물체가 돌아갑니다.
	static FQuaternion LookRotation(const FVector& forward, const FVector& up)
	{
		FVector F = forward.Normalized();
		// up을 F에 직교하도록 정규화(Gram-Schmidt)
		FVector Uref = up.Normalized();
		if (fabs(F.Dot(Uref)) > 0.999f) // 거의 평행
		{
			Uref = FVector(0, 1, 0); // 임시 다른 up (예: Y축)
		}
		// R = F × U, U = R × F
		FVector R = F.Cross(Uref).Normalized();
		FVector U = R.Cross(F).Normalized(); // 재직교(수치안정)
		FMatrix M = FMatrix::IdentityMatrix();
		// Unreal 좌표계: col0=Forward(X), col1=Right(Y), col2=Up(Z)
		M.M[0][0] = F.X; M.M[0][1] = R.X; M.M[0][2] = U.X;
		M.M[1][0] = F.Y; M.M[1][1] = R.Y; M.M[1][2] = U.Y;
		M.M[2][0] = F.Z; M.M[2][1] = R.Z; M.M[2][2] = U.Z;
		return FQuaternion::FromMatrixRow(M);
	}

	// 두 벡터 a→b 회전
	static FQuaternion FromTo(FVector a, FVector b)
	{
		a.Normalize(); b.Normalize();
		float c = a.Dot(b);
		if (c > 0.9999f) return Identity();
		if (c < -0.9999f)
		{
			// 반대 방향: a와 직교하는 축으로 180도
			FVector ortho = (fabsf(a.X) < 0.9f) ? FVector(1, 0, 0) : FVector(0, 1, 0);
			FVector axis = a.Cross(ortho).Normalized();
			return FromAxisAngle(axis, (float)PI);
		}
		FVector axis = a.Cross(b);
		float s = sqrtf((1.0f + c) * 2.0f);
		float invs = 1.0f / s;
		return FQuaternion(axis.X * invs, axis.Y * invs, axis.Z * invs, s * 0.5f).Normalized();
	}

	/*
	   벡터 회전 (쿼터니언 샌드위치)
	   단위 쿼터니언이 아니여도 된다.
	   해밀턴 곱은 연산량이 많아서 느립니다. 공부하실 때 참고해요
	*/
	/*
	FVector Rotate(const FVector& v) const
	{
		FQuaternion qv(v.X, v.Y, v.Z, 0.0f);
		FQuaternion inv = Inverse();
		// r = (q ⊗ v) ⊗ q^{-1}
		FQuaternion r = (*this) * qv * inv;      // (tmp, inv) => tmp ⊗ inv
		return FVector(r.X, r.Y, r.Z);
	}
	FVector RotateInverse(const FVector& v) const
	{
		FQuaternion qv(v.X, v.Y, v.Z, 0.0f);
		FQuaternion inv = Inverse();
		// r = (q ⊗ v) ⊗ q^{-1}
		FQuaternion r = inv * qv * (*this);      // (tmp, inv) => tmp ⊗ inv
		return FVector(r.X, r.Y, r.Z);
	}
	*/
	// 벡터 회전
	FVector Rotate(const FVector& v) const {
		float n2 = X * X + Y * Y + Z * Z + W * W;
		if (n2 <= 0.0f) return v;  // 안전가드

		FVector u(X, Y, Z);
		float  w = W;

		// v' = ((w^2 - |u|^2) v + 2 u (u·v) + 2 w (u×v)) / |q|^2
		float uu = u.Dot(u);
		float uv = u.Dot(v);

		FVector term1 = v * (w * w - uu);
		FVector term2 = u * (2.0f * uv);
		FVector term3 = (u.Cross(v)) * (2.0f * w);
		return (term1 + term2 + term3) * (1.0f / n2);
	}
	FVector RotateInverse(const FVector& v) const
	{
		const float n2 = X * X + Y * Y + Z * Z + W * W;
		if (n2 <= 0.0f) return v; // 안전 가드

		const FVector u(X, Y, Z);
		const float   w = W;

		const float uu = u.Dot(u);
		const float uv = u.Dot(v);

		const FVector term1 = v * (w * w - uu);
		const FVector term2 = u * (2.0f * uv);
		const FVector term3 = (u.Cross(v)) * (2.0f * w);
		return (term1 + term2 - term3) * (1.0f / n2);  // ← 여기만 '-' 로 바뀜
	}
	// 단위 쿼터니언이 확실하다면 이걸 쓰는게 더 빠릅니다
	FVector RotateUnit(const FVector& v) const
	{
		// q = (X,Y,Z,W), LH cross 기준
		FVector qv(X, Y, Z);
		FVector t = 2.0f * qv.Cross(v);
		return v + W * t + qv.Cross(t);
	}
	// 단위 쿼터니언이 확실하다면 이걸 쓰는게 더 빠릅니다
	FVector RotateUnitInverse(const FVector& v) const
	{
		const FVector u(X, Y, Z);
		const FVector t = 2.0f * u.Cross(v);
		return v - W * t + u.Cross(t);  // 정방향 v + W*t + u×t 와 w 항 부호만 반대
	}

	// === COLUMNS are axes (row-vector) ===
	// col0 = q·(+X) = Right
	FMatrix ToMatrixRow() const
	{
		FQuaternion q = Normalized();

		float x = q.X, y = q.Y, z = q.Z, w = q.W;
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;

		FMatrix R = FMatrix::IdentityMatrix();
		// === columns are axes (col0=+X, col1=+Y, col2=+Z) ===
		R.M[0][0] = 1.0f - 2.0f * (yy + zz);
		R.M[1][0] = 2.0f * (xy + wz);
		R.M[2][0] = 2.0f * (xz - wy);

		R.M[0][1] = 2.0f * (xy - wz);
		R.M[1][1] = 1.0f - 2.0f * (xx + zz);
		R.M[2][1] = 2.0f * (yz + wx);

		R.M[0][2] = 2.0f * (xz + wy);
		R.M[1][2] = 2.0f * (yz - wx);
		R.M[2][2] = 1.0f - 2.0f * (xx + yy);
		return R;
	}

	// 회전행렬(행벡터 규약, 상단3x3) → 쿼터니언
	static FQuaternion FromMatrixRow(const FMatrix& R)
	{
		float m00 = R.M[0][0], m01 = R.M[0][1], m02 = R.M[0][2];
		float m10 = R.M[1][0], m11 = R.M[1][1], m12 = R.M[1][2];
		float m20 = R.M[2][0], m21 = R.M[2][1], m22 = R.M[2][2];

		float tr = m00 + m11 + m22;
		FQuaternion q;
		if (tr > 0.0f)
		{
			float s = sqrtf(tr + 1.0f) * 2.0f;
			q.W = 0.25f * s;
			q.X = (m21 - m12) / s;
			q.Y = (m02 - m20) / s;
			q.Z = (m10 - m01) / s;
		}
		else if (m00 > m11 && m00 > m22)
		{
			float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
			q.W = (m21 - m12) / s;
			q.X = 0.25f * s;
			q.Y = (m01 + m10) / s;
			q.Z = (m02 + m20) / s;
		}

		else if (m11 > m22)
		{
			float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
			q.W = (m02 - m20) / s;
			q.X = (m01 + m10) / s;
			q.Y = 0.25f * s;
			q.Z = (m12 + m21) / s;
		}
		else
		{
			float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
			q.W = (m10 - m01) / s;
			q.X = (m02 + m20) / s;
			q.Y = (m12 + m21) / s;
			q.Z = 0.25f * s;
		}
		return q.Normalized();
	}
	// Return FVector(rx, ry, rz) rad(라디안)
	FVector GetEulerXYZ() const
	{
		// 규약: row-vector, columns are axes, R = Rx * Ry * Rz  (X→Y→Z 순서)
		FMatrix R = ToMatrixRow();
		// 행벡터 + columns=axes + R = Rx*Ry*Rz 전개 결과:
		// R[0][2] = sin(ry)
		// R = Rx * Ry * Rz (row-vector)
		const float sy = std::clamp(R.M[0][2], -1.0f, 1.0f); // +Y가 col1, 그래서 R[0][2]가 +sinY
		const float ry = asinf(sy);
		const float cy = cosf(ry);
		float rx, rz;
		if (fabsf(cy) > 1e-6f)
		{
			rx = atan2f(-R.M[1][2], R.M[2][2]); // X
			rz = atan2f(-R.M[0][1], R.M[0][0]); // Z
		}
		else
		{
			// gimbal lock
			rz = 0.0f;
			rx = (sy > 0.0f) ? atan2f(R.M[2][0], R.M[1][0])
				: atan2f(-R.M[2][0], -R.M[1][0]);
		}
		return FVector(rx, ry, rz);
	}
	// Return FVector(rx, ry, rz) deg(도)
	FVector GetEulerXYZDeg() const
	{
		return ToDeg(GetEulerXYZ());
	}
	// Return FVector(rx, ry, rz) rad
	static FVector GetEulerXYZ(const FQuaternion& q)
	{
		return q.GetEulerXYZ();
	}
	// Return FVector(rx, ry, rz) deg(도)
	static FVector GetEulerXYZDeg(const FQuaternion& q)
	{
		return ToDeg(q.GetEulerXYZ());
	}
	// 주어진 축(월드 축)을 기준으로 회전
	FQuaternion RotatedWorldAxisAngle(const FVector& worldAxis, float radians) const
	{
		FQuaternion qW = FromAxisAngle(worldAxis, radians);
		return (*this * qW).Normalized();
	}
	// 주어진 축(로컬 축) 기준으로 회전
	FQuaternion RotatedLocalAxisAngle(const FVector& LocalAxis, float radians) const
	{
		FQuaternion qL = FromAxisAngle(LocalAxis, radians);
		return (qL * *this).Normalized();
	}
	// In-place 버전
	void RotateWorldAxisAngle(const FVector& worldAxis, float radians)
	{
		*this = RotatedWorldAxisAngle(worldAxis, radians);
	}
	// In-place 버전
	void RotateLocalAxisAngle(const FVector& worldAxis, float radians)
	{
		*this = RotatedLocalAxisAngle(worldAxis, radians); 
	}
	// 월드 오일러(X→Y→Z)로 누적: q' = (Rx * Ry * Rz) * q
	FQuaternion RotatedWorldEulerXYZ(float rx, float ry, float rz) const
	{
		FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), rx);
		FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), ry);
		FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), rz);
		FQuaternion qW = qx * qy * qz;
		return (qW * *this).Normalized();
	}
	// 월드 오일러(X→Y→Z)로 누적: q' = (Rx * Ry * Rz) * q
	FQuaternion RotatedWorldEulerXYZ(const FVector& v) const
	{
		FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), v.X);
		FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), v.Y);
		FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), v.Z);
		FQuaternion qW = qx * qy * qz;
		return (qW * *this).Normalized();
	}
	void RotateWorldEulerXYZInPlace(float rx, float ry, float rz)
	{
		*this = RotatedWorldEulerXYZ(rx, ry, rz);
	}
	void RotateWorldEulerXYZInPlace(const FVector& v)
	{
		*this = RotatedWorldEulerXYZ(v.X, v.Y, v.Z);
	}
	// 월드 Yaw/Pitch/Roll 누적 (Z=Yaw, X=Pitch, Y=Roll; Z-up, Forward=+Y 규약)
	// 합성: Rz(yaw) * Rx(pitch) * Ry(roll)
	FQuaternion RotatedWorldYawPitchRoll(float yawZ, float pitchX, float rollY) const
	{
		FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), yawZ);
		FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), pitchX);
		FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), rollY);
		return (qz * qx * qy * *this).Normalized();
	}
	void RotateWorldYawPitchRollInPlace(float yawZ, float pitchX, float rollY)
	{
		*this = RotatedWorldYawPitchRoll(yawZ, pitchX, rollY);
	}

	// ============================================================
	// 월드 축 기준 누적 회전 (도 단위) — GUI용
	// ============================================================
	FQuaternion RotatedWorldAxisAngleDeg(const FVector& worldAxis, float degrees) const
	{
		return RotatedWorldAxisAngle(worldAxis, ToRad(degrees));
	}
	void RotateWorldAxisAngleDegInPlace(const FVector& worldAxis, float degrees)
	{
		*this = RotatedWorldAxisAngleDeg(worldAxis, degrees);
	}

	FQuaternion RotatedWorldEulerXYZDeg(float degX, float degY, float degZ) const
	{
		return RotatedWorldEulerXYZ(ToRad(degX), ToRad(degY), ToRad(degZ));
	}
	void RotateWorldEulerXYZDegInPlace(float degX, float degY, float degZ)
	{
		*this = RotatedWorldEulerXYZDeg(degX, degY, degZ);
	}

	FQuaternion RotatedWorldYawPitchRollDeg(float yawZ_deg, float pitchX_deg, float rollY_deg) const
	{
		return RotatedWorldYawPitchRoll(ToRad(yawZ_deg), ToRad(pitchX_deg), ToRad(rollY_deg));
	}
	void RotateWorldYawPitchRollDegInPlace(float yawZ_deg, float pitchX_deg, float rollY_deg)
	{
		*this = RotatedWorldYawPitchRollDeg(yawZ_deg, pitchX_deg, rollY_deg);
	}

	// ============================================================
	// 편의: 월드 축 단일 축(라디안) 회전
	// ============================================================
	FQuaternion RotatedWorldX(float radians) const { return RotatedWorldAxisAngle(FVector(1, 0, 0), radians); }
	FQuaternion RotatedWorldY(float radians) const { return RotatedWorldAxisAngle(FVector(0, 1, 0), radians); }
	FQuaternion RotatedWorldZ(float radians) const { return RotatedWorldAxisAngle(FVector(0, 0, 1), radians); }
	void RotateWorldXInPlace(float radians) { *this = RotatedWorldX(radians); }
	void RotateWorldYInPlace(float radians) { *this = RotatedWorldY(radians); }
	void RotateWorldZInPlace(float radians) { *this = RotatedWorldZ(radians); }

	// 도 단위 단일 축
	FQuaternion RotatedWorldXDeg(float deg) const { return RotatedWorldX(ToRad(deg)); }
	FQuaternion RotatedWorldYDeg(float deg) const { return RotatedWorldY(ToRad(deg)); }
	FQuaternion RotatedWorldZDeg(float deg) const { return RotatedWorldZ(ToRad(deg)); }
	void RotateWorldXDegInPlace(float deg) { *this = RotatedWorldXDeg(deg); }
	void RotateWorldYDegInPlace(float deg) { *this = RotatedWorldYDeg(deg); }
	void RotateWorldZDegInPlace(float deg) { *this = RotatedWorldZDeg(deg); }
};

// 참고용
/*
// 모델행렬 (row-vector): M = S * R * T
inline FMatrix MakeModelRow(const FVector& pos, const FQuaternion& rot, const FVector& scl)
{
	FMatrix S = FMatrix::Scale(scl.X, scl.Y, scl.Z);
	FMatrix R = rot.ToMatrixRow();
	FMatrix T = FMatrix::TranslationRow(pos.X, pos.Y, pos.Z);
	return S * R * T;
}
*/

