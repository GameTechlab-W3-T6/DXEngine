#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Vector4.h"
#include "UEngineStatics.h"

//struct FQuaternion; // 전방 선언

/**
 * @brief 4x4 matrix structure for 3D transformations and projections
 */
struct FMatrix
{
	float M[4][4];
	// ===== 생성/기본 =====
	FMatrix() { SetIdentity(); }
	// 대각 행렬(모든 대각 원소가 동일한 값 v일 때는 스칼라 행렬)로 초기화 (대각선에 값, 나머지는 0)
	FMatrix(float v) { for (int32 r = 0; r < 4; ++r) for (int32 c = 0; c < 4; ++c) M[r][c] = (r == c) ? v : 0.0f; }
	static FMatrix IdentityMatrix() { FMatrix t; t.SetIdentity(); return t; }
	void SetIdentity()
	{
		for (int32 r = 0; r < 4; ++r) for (int32 c = 0; c < 4; ++c) M[r][c] = (r == c) ? 1.0f : 0.0f;
	}
	static const FMatrix Identity; // 정의는 .cpp에: const FMatrix FMatrix::Identity = FMatrix::IdentityMatrix();
	// ===== 행렬 기본 연산 =====
	static FMatrix Multiply(const FMatrix& A, const FMatrix& B)
	{
		FMatrix R(0.0f);
		for (int32 r = 0; r < 4; ++r)
			for (int32 c = 0; c < 4; ++c)
				for (int32 k = 0; k < 4; ++k)
					R.M[r][c] += A.M[r][k] * B.M[k][c];
		return R;
	}

	static FVector4 MultiplyVector(FMatrix m, FVector4 v)
	{
		FVector4 R(0, 0, 0, 0);
		for (int32 i = 0; i < 4; i++)
		{
			for (int32 j = 0; j < 4; j++)
			{
				R[i] += m.M[i][j] * v[j];
			}
		}
		return R;
	}

	// 행우선
	static FVector4 MultiplyVectorRow(FVector4 v, FMatrix m)
	{
		FVector4 R(0, 0, 0, 0);

		// 루프 순서 변경: j가 바깥쪽, i가 안쪽
		for (int32 j = 0; j < 4; j++) // 결과 벡터의 각 성분(x, y, z, w)을 계산
		{
			for (int32 i = 0; i < 4; i++) // 입력 벡터와 행렬의 한 '열'을 순회
			{
				// R[j]는 결과 벡터의 j번째 성분 (예: j=0이면 x)
				// v[i]는 입력 벡터의 i번째 성분 (예: i=0이면 x)
				// m.M[i][j]는 행렬의 i행 j열 성분
				R[j] += v[i] * m.M[i][j];
			}
		}
		return R;
	}

	FMatrix operator*(const FMatrix& B) const { return Multiply(*this, B); }
	static FMatrix Transpose(const FMatrix& A)
	{
		FMatrix T(0.0f);
		for (int32 r = 0; r < 4; ++r) for (int32 c = 0; c < 4; ++c) T.M[r][c] = A.M[c][r];
		return T;
	}
	// ===== 벡터 변환 (column-vector / row-vector) =====
	// Column-vector: v' = M * [x y z w]^T
	FVector TransformPoint(const FVector& p, float w = 1.0f) const
	{
		float x = M[0][0] * p.X + M[0][1] * p.Y + M[0][2] * p.Z + M[0][3] * w;
		float y = M[1][0] * p.X + M[1][1] * p.Y + M[1][2] * p.Z + M[1][3] * w;
		float z = M[2][0] * p.X + M[2][1] * p.Y + M[2][2] * p.Z + M[2][3] * w;
		float ww = M[3][0] * p.X + M[3][1] * p.Y + M[3][2] * p.Z + M[3][3] * w;
		// 원근 투영 행렬을 거치면 w가 1이 아닌 값으로 바뀌고, 이때 꼭 나눠야 화면에 올바르게 투영된다.
		if (ww != 0.0f && ww != 1.0f) { x /= ww; y /= ww; z /= ww; }
		return FVector(x, y, z);
	}
	FVector TransformVector(const FVector& v) const
	{ // w=0
		float x = M[0][0] * v.X + M[0][1] * v.Y + M[0][2] * v.Z;
		float y = M[1][0] * v.X + M[1][1] * v.Y + M[1][2] * v.Z;
		float z = M[2][0] * v.X + M[2][1] * v.Y + M[2][2] * v.Z;
		return FVector(x, y, z);
	}
	// Row-vector: v'^T = [x y z w] * M
	FVector TransformPointRow(const FVector& p, float w = 1.0f) const
	{
		float x = p.X * M[0][0] + p.Y * M[1][0] + p.Z * M[2][0] + w * M[3][0];
		float y = p.X * M[0][1] + p.Y * M[1][1] + p.Z * M[2][1] + w * M[3][1];
		float z = p.X * M[0][2] + p.Y * M[1][2] + p.Z * M[2][2] + w * M[3][2];
		float ww = p.X * M[0][3] + p.Y * M[1][3] + p.Z * M[2][3] + w * M[3][3];
		if (ww != 0.0f && ww != 1.0f) { x /= ww; y /= ww; z /= ww; }
		return FVector(x, y, z);
	}
	FVector TransformVectorRow(const FVector& v) const
	{
		float x = v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0];
		float y = v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1];
		float z = v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2];
		return FVector(x, y, z);
	}
	FVector4 TransformVectorRow(const FVector4& v) const
	{
		float x = v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0] + v.W * M[3][0];
		float y = v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1] + v.W * M[3][1];
		float z = v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2] + v.W * M[3][2];
		float w = v.X * M[0][3] + v.Y * M[1][3] + v.Z * M[2][3] + v.W * M[3][3];
		return FVector4(x, y, z, w);
	}
	// ===== 행렬 성질/도구 =====
	static float Det3(float a00, float a01, float a02,
		float a10, float a11, float a12,
		float a20, float a21, float a22)
	{
		return a00 * (a11 * a22 - a12 * a21)
			- a01 * (a10 * a22 - a12 * a20)
			+ a02 * (a10 * a21 - a11 * a20);
	}
	float Determinant() const
	{
		const float a00 = M[0][0], a01 = M[0][1], a02 = M[0][2], a03 = M[0][3];
		const float a10 = M[1][0], a11 = M[1][1], a12 = M[1][2], a13 = M[1][3];
		const float a20 = M[2][0], a21 = M[2][1], a22 = M[2][2], a23 = M[2][3];
		const float a30 = M[3][0], a31 = M[3][1], a32 = M[3][2], a33 = M[3][3];

		float c0 = Det3(a11, a12, a13, a21, a22, a23, a31, a32, a33);
		float c1 = Det3(a10, a12, a13, a20, a22, a23, a30, a32, a33);
		float c2 = Det3(a10, a11, a13, a20, a21, a23, a30, a31, a33);
		float c3 = Det3(a10, a11, a12, a20, a21, a22, a30, a31, a32);
		return a00 * c0 - a01 * c1 + a02 * c2 - a03 * c3;
	}
	static FMatrix Adjugate(const FMatrix& A)
	{
		FMatrix C; // cofactor matrix, then transpose
		const float a00 = A.M[0][0], a01 = A.M[0][1], a02 = A.M[0][2], a03 = A.M[0][3];
		const float a10 = A.M[1][0], a11 = A.M[1][1], a12 = A.M[1][2], a13 = A.M[1][3];
		const float a20 = A.M[2][0], a21 = A.M[2][1], a22 = A.M[2][2], a23 = A.M[2][3];
		const float a30 = A.M[3][0], a31 = A.M[3][1], a32 = A.M[3][2], a33 = A.M[3][3];

		float cof[4][4];
		cof[0][0] = Det3(a11, a12, a13, a21, a22, a23, a31, a32, a33);
		cof[0][1] = -Det3(a10, a12, a13, a20, a22, a23, a30, a32, a33);
		cof[0][2] = Det3(a10, a11, a13, a20, a21, a23, a30, a31, a33);
		cof[0][3] = -Det3(a10, a11, a12, a20, a21, a22, a30, a31, a32);

		cof[1][0] = -Det3(a01, a02, a03, a21, a22, a23, a31, a32, a33);
		cof[1][1] = Det3(a00, a02, a03, a20, a22, a23, a30, a32, a33);
		cof[1][2] = -Det3(a00, a01, a03, a20, a21, a23, a30, a31, a33);
		cof[1][3] = Det3(a00, a01, a02, a20, a21, a22, a30, a31, a32);

		cof[2][0] = Det3(a01, a02, a03, a11, a12, a13, a31, a32, a33);
		cof[2][1] = -Det3(a00, a02, a03, a10, a12, a13, a30, a32, a33);
		cof[2][2] = Det3(a00, a01, a03, a10, a11, a13, a30, a31, a33);
		cof[2][3] = -Det3(a00, a01, a02, a10, a11, a12, a30, a31, a32);

		cof[3][0] = -Det3(a01, a02, a03, a11, a12, a13, a21, a22, a23);
		cof[3][1] = Det3(a00, a02, a03, a10, a12, a13, a20, a22, a23);
		cof[3][2] = -Det3(a00, a01, a03, a10, a11, a13, a20, a21, a23);
		cof[3][3] = Det3(a00, a01, a02, a10, a11, a12, a20, a21, a22);

		// adjugate = transpose of cofactor
		for (int32 r = 0; r < 4; ++r) for (int32 c = 0; c < 4; ++c) C.M[r][c] = cof[c][r];
		return C;
	}
	static FMatrix Inverse(const FMatrix& A, bool* ok = nullptr)
	{
		float det = A.Determinant();
		if (ok) *ok = (det != 0.0f);
		if (det == 0.0f) { FMatrix Z(0.0f); return Z; }
		FMatrix adj = Adjugate(A);
		const float invDet = 1.0f / det;
		for (int32 r = 0; r < 4; ++r) for (int32 c = 0; c < 4; ++c) adj.M[r][c] *= invDet;
		return adj;
	}

	// ===== 정규/직교 검사 =====
	// 직교(orthogonal): R^T * R = I  (회전 행렬)
	// 오차 허용이 있으므로 주의해야할 필요가 있다.
	bool IsOrthogonal(float eps = 1e-4f) const
	{
		FMatrix Rt = Transpose(*this);
		FMatrix I = Rt * (*this);
		for (int32 r = 0; r < 3; ++r) for (int32 c = 0; c < 3; ++c)
		{
			float target = (r == c) ? 1.0f : 0.0f;
			if (fabsf(I.M[r][c] - target) > eps) return false;
		}
		return true;
	}
	// 정규직교(orthonormal): 위 + 각 축 길이 1
	// 사실 위의 IsOrthogonal 검사에서 이미 각 축 길이 1인지 확인하므로 동일하긴 하다.
	// 벡터 집합에서는 직교와 정규 직교가 다르지만, 행렬에서는 직교 행렬이 곧 정규직교 행렬이다.
	bool IsOrthonormal(float eps = 1e-4f) const { return IsOrthogonal(eps); }
	// 정규(normal) 행렬 (그래픽스 관례): 법선 변환용 N = (M^{-1})^T 의 상단 3x3
	static FMatrix NormalMatrix(const FMatrix& Model)
	{
		FMatrix inv = Inverse(Model);
		FMatrix n = Transpose(inv);
		// 하단 행/열 정리
		n.M[0][3] = n.M[1][3] = n.M[2][3] = 0.0f;
		n.M[3][0] = n.M[3][1] = n.M[3][2] = 0.0f;
		n.M[3][3] = 1.0f;
		return n;
	}
	// ===== 기본 변환 생성 =====
	static FMatrix Translation(float tx, float ty, float tz)
	{
		FMatrix T = IdentityMatrix();
		T.M[0][3] = tx; T.M[1][3] = ty; T.M[2][3] = tz;
		return T;
	}
	static FMatrix TranslationRow(float tx, float ty, float tz)
	{
		FMatrix T = IdentityMatrix();
		T.M[3][0] = tx; T.M[3][1] = ty; T.M[3][2] = tz; // row 규약
		return T;
	}
	static FMatrix Scale(float sx, float sy, float sz)
	{
		FMatrix S(0.0f);
		S.M[0][0] = sx; S.M[1][1] = sy; S.M[2][2] = sz; S.M[3][3] = 1.0f;
		return S;
	}
	static FMatrix RotationX(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[1][1] = c; R.M[1][2] = -s;
		R.M[2][1] = s; R.M[2][2] = c;
		return R;
	}
	static FMatrix RotationY(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[0][0] = c; R.M[0][2] = s;
		R.M[2][0] = -s; R.M[2][2] = c;
		return R;
	}
	static FMatrix RotationZ(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[0][0] = c; R.M[0][1] = -s;
		R.M[1][0] = s; R.M[1][1] = c;
		return R;
	}
	static FMatrix RotationAxis(const FVector& axis, float rad)
	{
		// 단위축 가정 (필요 시 노말라이즈)
		float x = axis.X, y = axis.Y, z = axis.Z;
		float len = sqrtf(x * x + y * y + z * z); if (len == 0) return IdentityMatrix();
		x /= len; y /= len; z /= len;
		float c = cosf(rad), s = sinf(rad), t = 1.0f - c;

		FMatrix R(0.0f);
		R.M[0][0] = t * x * x + c;   R.M[0][1] = t * x * y - s * z; R.M[0][2] = t * x * z + s * y; R.M[0][3] = 0;
		R.M[1][0] = t * y * x + s * z; R.M[1][1] = t * y * y + c; R.M[1][2] = t * y * z - s * x; R.M[1][3] = 0;
		R.M[2][0] = t * z * x - s * y; R.M[2][1] = t * z * y + s * x; R.M[2][2] = t * z * z + c; R.M[2][3] = 0;
		R.M[3][0] = 0; R.M[3][1] = 0; R.M[3][2] = 0; R.M[3][3] = 1;
		return R;
	}
	static FMatrix RotationXRow(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[1][1] = c;  R.M[1][2] = s;
		R.M[2][1] = -s; R.M[2][2] = c;
		return R;
	}
	static FMatrix RotationYRow(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[0][0] = c;  R.M[0][2] = -s;
		R.M[2][0] = s;  R.M[2][2] = c;
		return R;
	}
	static FMatrix RotationZRow(float rad)
	{
		float c = cosf(rad), s = sinf(rad);
		FMatrix R = IdentityMatrix();
		R.M[0][0] = c;  R.M[0][1] = s;
		R.M[1][0] = -s; R.M[1][1] = c;
		return R;
	}
	static FMatrix RotationAxisRow(const FVector& axis, float rad)
	{
		float len2 = axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z;
		if (len2 < 1e-8f) return IdentityMatrix();
		float invLen = 1.0f / sqrtf(len2);
		float x = axis.X * invLen, y = axis.Y * invLen, z = axis.Z * invLen;
		float c = cosf(rad), s = sinf(rad), t = 1.0f - c;

		FMatrix R(0.0f);
		R.M[0][0] = t * x * x + c;   R.M[0][1] = t * x * y + s * z; R.M[0][2] = t * x * z - s * y; R.M[0][3] = 0;
		R.M[1][0] = t * y * x - s * z; R.M[1][1] = t * y * y + c;   R.M[1][2] = t * y * z + s * x; R.M[1][3] = 0;
		R.M[2][0] = t * z * x + s * y; R.M[2][1] = t * z * y - s * x; R.M[2][2] = t * z * z + c;   R.M[2][3] = 0;
		R.M[3][0] = 0;           R.M[3][1] = 0;           R.M[3][2] = 0;           R.M[3][3] = 1;
		return R;
	}
	// ===== View 행렬 (LookAt) =====
	// RH: +Z가 위 그림처럼 "위"이고, 카메라 앞은 -Z를 바라보는 전통(OpenGL식)과 다를 수 있으므로
	// 여기선 명시적으로 구현
	static FMatrix LookAtRH(const FVector& eye, const FVector& target, const FVector& up)
	{
		// f = normalize(eye - target)  (카메라가 바라보는 방향 = -forward)
		FVector f(eye.X - target.X, eye.Y - target.Y, eye.Z - target.Z);
		float fl = sqrtf(f.X * f.X + f.Y * f.Y + f.Z * f.Z); f.X /= fl; f.Y /= fl; f.Z /= fl;
		// s = normalize(up × f)
		FVector s(
			up.Y * f.Z - up.Z * f.Y,
			up.Z * f.X - up.X * f.Z,
			up.X * f.Y - up.Y * f.X
		);
		float sl = sqrtf(s.X * s.X + s.Y * s.Y + s.Z * s.Z); s.X /= sl; s.Y /= sl; s.Z /= sl;
		// u = f × s
		FVector u(
			f.Y * s.Z - f.Z * s.Y,
			f.Z * s.X - f.X * s.Z,
			f.X * s.Y - f.Y * s.X
		);
		FMatrix V(0.0f);
		V.M[0][0] = s.X; V.M[0][1] = s.Y; V.M[0][2] = s.Z; V.M[0][3] = -(s.X * eye.X + s.Y * eye.Y + s.Z * eye.Z);
		V.M[1][0] = u.X; V.M[1][1] = u.Y; V.M[1][2] = u.Z; V.M[1][3] = -(u.X * eye.X + u.Y * eye.Y + u.Z * eye.Z);
		V.M[2][0] = f.X; V.M[2][1] = f.Y; V.M[2][2] = f.Z; V.M[2][3] = -(f.X * eye.X + f.Y * eye.Y + f.Z * eye.Z);
		V.M[3][0] = 0;   V.M[3][1] = 0;   V.M[3][2] = 0;   V.M[3][3] = 1;
		return V;
	}
	static FMatrix LookAtRHRow(const FVector& eye, const FVector& target, const FVector& up)
	{
		FVector f(eye.X - target.X, eye.Y - target.Y, eye.Z - target.Z); // -forward
		f.Normalize();
		FVector s(
			up.Cross(f)
		);
		s.Normalize();
		FVector u(
			f.Cross(s)
		);
		FMatrix V = IdentityMatrix();
		// 아래 세 줄은 회전성분을 채우는 것이다.  (카메라의 x,y,z 축이 월드 좌표계에서 어디를 향하는지)
		// camera's axis
		V.M[0][0] = s.X; V.M[0][1] = u.X; V.M[0][2] = f.X; V.M[0][3] = 0;
		V.M[1][0] = s.Y; V.M[1][1] = u.Y; V.M[1][2] = f.Y; V.M[1][3] = 0;
		V.M[2][0] = s.Z; V.M[2][1] = u.Z; V.M[2][2] = f.Z; V.M[2][3] = 0;
		// translation(카메라의 위치를 카메라의 x,y,z 축으로 본 좌표의 음수다.)
		// 월드 좌표의 eye 위치를 뷰 공간 원점으로 이동시키는 역할
		V.M[3][0] = -(eye.X * s.X + eye.Y * s.Y + eye.Z * s.Z);
		V.M[3][1] = -(eye.X * u.X + eye.Y * u.Y + eye.Z * u.Z);
		V.M[3][2] = -(eye.X * f.X + eye.Y * f.Y + eye.Z * f.Z);
		V.M[3][3] = 1;
		return V;
	}
	static FMatrix LookAtLH(const FVector& eye, const FVector& target, const FVector& up)
	{
		// f = normalize(target - eye)
		FVector f(eye.X - target.X, eye.Y - target.Y, eye.Z - target.Z); // -forward
		f.Normalize();
		FVector s(
			up.Cross(f)
		);
		s.Normalize();
		FVector u(
			f.Cross(s)
		);
		FMatrix V = IdentityMatrix();
		// 아래 세 줄은 회전성분을 채우는 것이다.  (카메라의 x,y,z 축이 월드 좌표계에서 어디를 향하는지)
		// camera's axis
		V.M[0][0] = s.X; V.M[0][1] = u.X; V.M[0][2] = f.X; V.M[0][3] = 0;
		V.M[1][0] = s.Y; V.M[1][1] = u.Y; V.M[1][2] = f.Y; V.M[1][3] = 0;
		V.M[2][0] = s.Z; V.M[2][1] = u.Z; V.M[2][2] = f.Z; V.M[2][3] = 0;
		// translation(카메라의 위치를 카메라의 x,y,z 축으로 본 좌표의 음수다.)
		// 월드 좌표의 eye 위치를 뷰 공간 원점으로 이동시키는 역할
		V.M[3][0] = -(eye.X * s.X + eye.Y * s.Y + eye.Z * s.Z);
		V.M[3][1] = -(eye.X * u.X + eye.Y * u.Y + eye.Z * u.Z);
		V.M[3][2] = -(eye.X * f.X + eye.Y * f.Y + eye.Z * f.Z);
		V.M[3][3] = 1;
		return V;
	}
	static FMatrix LookAtLHRow(const FVector& eye, const FVector& target, const FVector& up)
	{
		// f: 카메라 앞 방향 (LH: target - eye)
		FVector f(target.X - eye.X, target.Y - eye.Y, target.Z - eye.Z);
		f.Normalize();

		// s: 오른쪽 (right) = normalize(up × f)  ← LH에서 표준
		FVector s = up.Cross(f);
		s.Normalize();

		// u: 위 (up') = s × f
		FVector u = f.Cross(s);

		FMatrix V = IdentityMatrix();
		// 회전 성분 (row-vector 규약: 카메라 축을 각 열에 배치)
		V.M[0][0] = s.X; V.M[0][1] = u.X; V.M[0][2] = f.X; V.M[0][3] = 0.0f;
		V.M[1][0] = s.Y; V.M[1][1] = u.Y; V.M[1][2] = f.Y; V.M[1][3] = 0.0f;
		V.M[2][0] = s.Z; V.M[2][1] = u.Z; V.M[2][2] = f.Z; V.M[2][3] = 0.0f;

		// 평행이동 (row-vector: 마지막 행)
		V.M[3][0] = -(eye.X * s.X + eye.Y * s.Y + eye.Z * s.Z);
		V.M[3][1] = -(eye.X * u.X + eye.Y * u.Y + eye.Z * u.Z);
		V.M[3][2] = -(eye.X * f.X + eye.Y * f.Y + eye.Z * f.Z);
		V.M[3][3] = 1.0f;
		return V;
	}
	// ===== Projection =====
	static FMatrix PerspectiveFovRH(float fovY, float aspect, float zNear, float zFar)
	{
		float f = 1.0f / tanf(fovY * 0.5f);
		FMatrix P(0.0f);
		P.M[0][0] = f / aspect;
		P.M[1][1] = f;
		P.M[2][2] = zFar / (zNear - zFar);
		P.M[2][3] = (zFar * zNear) / (zNear - zFar);
		P.M[3][2] = -1.0f;
		return P;
	}

	// === Perspective (RH, row-vector, D3D식 depth[0,1]) ===
	static FMatrix PerspectiveFovRHRow(float fovY, float aspect, float zn, float zf)
	{
		float f = 1.0f / tanf(fovY * 0.5f);
		FMatrix P(0.0f);
		P.M[0][0] = f / aspect;
		P.M[1][1] = f;
		// 음수여야 아래 뒤집기로 인해 양수가 된다.
		P.M[2][2] = zf / (zn - zf);
		// 뒤집기 (카메라는 -z 를 바라보므로)
		P.M[2][3] = -1.0f;
		P.M[3][2] = (zn * zf) / (zn - zf);
		return P;
	}

	static FMatrix PerspectiveFovLH(float fovY, float aspect, float zNear, float zFar)
	{
		float f = 1.0f / tanf(fovY * 0.5f);
		FMatrix P(0.0f);
		P.M[0][0] = f / aspect;
		P.M[1][1] = f;
		P.M[2][2] = zFar / (zFar - zNear);
		P.M[2][3] = (-zNear * zFar) / (zFar - zNear);
		P.M[3][2] = 1.0f;
		return P;
	}
	static FMatrix PerspectiveFovLHRow(float fovY, float aspect, float zn, float zf)
	{
		float f = 1.0f / tanf(fovY * 0.5f);
		FMatrix P(0.0f);
		P.M[0][0] = f / aspect;
		P.M[1][1] = f;
		P.M[2][2] = zf / (zf - zn);
		P.M[2][3] = 1.0f;                          // row-vector에서는 여기가 1
		P.M[3][2] = (-zn * zf) / (zf - zn);
		// P.M[3][3] = 0 (이미 0)
		return P;
	}
	// === Orthographic (RH, row-vector, D3D식 depth[0,1]) ===
	static FMatrix OrthoRHRow(float w, float h, float zn, float zf)
	{
		FMatrix O(0.0f);
		O.M[0][0] = 2.0f / w;
		O.M[1][1] = 2.0f / h;
		O.M[2][2] = 1.0f / (zn - zf);         // 음수
		O.M[3][2] = zn / (zn - zf);
		O.M[3][3] = 1.0f;
		return O;
	}
	static FMatrix OrthoLHRow(float w, float h, float zn, float zf)
	{
		FMatrix O(0.0f);
		O.M[0][0] = 2.0f / w;
		O.M[1][1] = 2.0f / h;
		O.M[2][2] = 1.0f / (zf - zn);
		O.M[2][3] = -zn / (zf - zn);   // row-vector에선 [2][3]
		O.M[3][3] = 1.0f;
		return O;
	}
	// ===== TRS 조합 (모델 행렬) =====
	// Column-vector 기준: M = T * R * S (v' = M * v)
	static FMatrix TRS(const FVector& t, const FVector& rRadXYZ, const FVector& s)
	{
		FMatrix T = Translation(t.X, t.Y, t.Z);
		FMatrix Rx = RotationX(rRadXYZ.X);
		FMatrix Ry = RotationY(rRadXYZ.Y);
		FMatrix Rz = RotationZ(rRadXYZ.Z);
		FMatrix R = Rz * (Ry * Rx); // Z*Y*X (엔진 취향에 맞게 조절)
		FMatrix S = Scale(s.X, s.Y, s.Z);
		return T * (R * S);
	}
	// v' = v * (S * R * T)
	static FMatrix TRSRow(const FVector& t, const FVector& rRadXYZ, const FVector& s)
	{
		FMatrix S = Scale(s.X, s.Y, s.Z);
		FMatrix Rx = RotationXRow(rRadXYZ.X);
		FMatrix Ry = RotationYRow(rRadXYZ.Y);
		FMatrix Rz = RotationZRow(rRadXYZ.Z);
		FMatrix R = Rx * Ry * Rz;               // 필요에 따라 오일러 순서 변경 가능
		FMatrix T = TranslationRow(t.X, t.Y, t.Z);
		return S * R * T;                        // row 규약 핵심
	}

	static FMatrix SRTRowEuler(const FVector& t, const FVector& eulerXYZ, const FVector& s)
	{
		FMatrix S = Scale(s.X, s.Y, s.Z);
		FMatrix Rx = RotationXRow(eulerXYZ.X * DegreeToRadian);
		FMatrix Ry = RotationYRow(eulerXYZ.Y * DegreeToRadian);
		FMatrix Rz = RotationZRow(eulerXYZ.Z * DegreeToRadian);
		FMatrix R = Rx * Ry * Rz;               // 필요에 따라 오일러 순서 변경 가능
		FMatrix T = TranslationRow(t.X, t.Y, t.Z);
		return S * R * T;                        // row 규약 핵심
	}

	static FMatrix SRTRowQuaternion(const FVector& t, const FMatrix& R, const FVector& s)
	{
		FMatrix S = Scale(s.X, s.Y, s.Z);
		FMatrix T = TranslationRow(t.X, t.Y, t.Z);
		return S * R * T;                        // row 규약 핵심
	}
};