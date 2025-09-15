#include "stdafx.h"
#include "GizmoVertices.h"

#include "Vector4.h"

const float cylRadius = 0.035;        // 원기둥 반지름
const float cylHeight = 0.8;        // 원기둥 높이
const float coneHeight = 0.25;       // 원뿔 높이
const float coneRadius = 0.125;		// 원뿔 반지름

const TArray<FVertexPosColor> gizmo_arrow_vertices =
{
	// 원기둥 바닥 (총 8개 삼각형, LH z-up CW 와인딩)
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, 0.0f, cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, 0.0f, -cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, -cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },

	// 원기둥 옆면 (총 16개 삼각형, LH z-up CW 와인딩)
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, 0.0f, cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, cylHeight, cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, 0.0f, -cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, cylHeight, -cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, -cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, 0.0f, -cylRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, -cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, -cylRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ cylRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },

	// 원뿔 밑면 (총 8개 삼각형, LH z-up CW 와인딩)
	{ coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { coneRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, cylHeight, coneRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, coneRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ -coneRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -coneRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ 0.0f, cylHeight, -coneRadius, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },
	{ coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, -coneRadius, 0.8f, 0.8f, 0.8f, 1 },
	{ coneRadius, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { 0.0f, cylHeight, 0.0f, 0.8f, 0.8f, 0.8f, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 0.8f, 0.8f, 0.8f, 1 },

	// 원뿔 옆면 (총 8개 삼각형, LH z-up CW 와인딩)
	{ coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }
};

TArray<FVertexPosColor> GridGenerator::CreateRotationHandleVertices()
{
	const int32 segU = 30; // 도넛 둘레 분할
	const int32 segV = 3;  // 단면 분할
	const float R = 1; // 큰 반지름
	const float r = 0.05f;       // 작은 반지름

	TArray<FVertexPosColor> verts;
	auto AddVertex = [&](float x, float y, float z)
		{
			verts.push_back({ x, y, z, 0.9f, 0.9f, 0.9f, 1 });
		};

	// 토러스 정점 생성
	for (int32 i = 0; i < segU; i++)
	{
		float u0 = i * 2.0f * PI / segU;
		float u1 = (i + 1) * 2.0f * PI / segU;

		for (int32 j = 0; j < segV; j++)
		{
			// 바닥면을 안그려서 뒷면 안보이게
			if (j == 1)
			{
				continue;
			}

			float v0 = j * 2.0f * PI / segV;
			float v1 = (j + 1) * 2.0f * PI / segV;

			// 4개 꼭짓점 (Y축 중심, XZ 평면에 위치)
			FVector p0(-((R + r * cosf(v0)) * sinf(u0)), // x' = -y
				r * sinf(v0),                      // y' = x
				-((R + r * cosf(v0)) * cosf(u0)));  // z' = z

			FVector p1(-((R + r * cosf(v0)) * sinf(u1)),
				r * sinf(v0),
				-((R + r * cosf(v0)) * cosf(u1)));

			FVector p2(-((R + r * cosf(v1)) * sinf(u0)),
				r * sinf(v1),
				-((R + r * cosf(v1)) * cosf(u0)));

			FVector p3(-((R + r * cosf(v1)) * sinf(u1)),
				r * sinf(v1),
				-((R + r * cosf(v1)) * cosf(u1)));

			// LH z-up CW 와인딩 삼각형 두 개
			AddVertex(p0.X, p0.Y, p0.Z);
			AddVertex(p1.X, p1.Y, p1.Z);
			AddVertex(p2.X, p2.Y, p2.Z);

			AddVertex(p1.X, p1.Y, p1.Z);
			AddVertex(p3.X, p3.Y, p3.Z);
			AddVertex(p2.X, p2.Y, p2.Z);
		}
	}

	return verts;
}

const float boxWidth = 0.05f; // 사각기둥의 너비 (X, Z 방향)
const float boxHeight = 0.8f; // 사각기둥의 높이 (Y 방향)
const float cubeSize = 0.2f;  // 정육면체의 한 변 길이
const float cubeOffset = boxHeight + (cubeSize / 2.0f); // 정육면체 중심의 Y 오프셋 (사각기둥 끝에 붙이도록)

const TArray<FVertexPosColor> gizmo_scale_handle_vertices =
{
	// ===================================================================================
	// 사각기둥 (Box/Prism) - 12개 삼각형 (LH z-up CW 와인딩)
	// ===================================================================================

	// 아랫면 (-Y)
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 윗면 (+Y, 사각기둥의 끝)
	{ -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 옆면 1 (앞, +Z)
	{ -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 옆면 2 (뒤, -Z)
	{ boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 옆면 3 (오른쪽, +X)
	{ boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 옆면 4 (왼쪽, -X)
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// ===================================================================================
	// 정육면체 (Cube) - 12개 삼각형 (LH z-up CW 와인딩)
	// ===================================================================================

	// 아랫면 (정육면체 기준 -Y)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 0.8f, 0.8f, 0.8f, 1 },

	// 윗면 (정육면체 기준 +Y)
	{ -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 1 (앞, +Z)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 2 (뒤, -Z)
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 3 (오른쪽, +X)
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 4 (왼쪽, -X)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }
};
TArray<FVertexPosColor> GridGenerator::CreateGridVertices(float gridSize, int32 gridCount)
{
	// TArray 컨테이너 생성
	TArray<FVertexPosColor> vertices;

	FVector4 normalColor(0.5f, 0.5f, 0.5f, 1.0f);  // grey
	FVector4 highlightColor(0.85f, 0.85f, 0.85f, 1.0f); // whiter

	// Z축에 평행한 세로 선들을 생성하는 로직
	for (int32 i = -gridCount; i <= gridCount; ++i)
	{
		if (i == 0) continue; // skip the central X axis line
		FVector4 color = (i % 5 == 0) ? highlightColor : normalColor;
		vertices.push_back({ -gridSize * gridCount, 0.0f, i * gridSize, color.X, color.Y, color.Z, color.W });
		vertices.push_back({ gridSize * gridCount, 0.0f, i * gridSize, color.X, color.Y, color.Z, color.W });
	}

	// X축에 평행한 가로 선들을 생성하는 로직
	for (int32 i = -gridCount; i <= gridCount; ++i)
	{
		if (i == 0) continue; // skip the central Z axis line
		FVector4 color = (i % 5 == 0) ? highlightColor : normalColor;
		vertices.push_back({ i * gridSize, 0.0f, -gridSize * gridCount, color.X, color.Y, color.Z, color.W });
		vertices.push_back({ i * gridSize, 0.0f, gridSize * gridCount, color.X, color.Y, color.Z, color.W });
	}

	// --------------------------
	// Main axis lines (X, Y, Z)
	// --------------------------

	float axisLength = gridSize * gridCount;

	// Positive side colors (strong RGB)
	FVector4 xPos(1.0f, 0.0f, 0.0f, 1.0f); // red
	FVector4 yPos(0.0f, 1.0f, 0.0f, 1.0f); // green
	FVector4 zPos(0.0f, 0.0f, 1.0f, 1.0f); // blue

	// Negative side colors (lighter/desaturated)
	FVector4 xNeg(0.5f, 0.3f, 0.3f, 1.0f); // light red
	FVector4 yNeg(0.3f, 0.45f, 0.3f, 1.0f); // light green
	FVector4 zNeg(0.3f, 0.3f, 0.5f, 1.0f); // light blue

	// X axis
	vertices.push_back({ 0.0f, 0.0f, 0.0f, xNeg.X, xNeg.Y, xNeg.Z, xNeg.W });
	vertices.push_back({ -axisLength, 0.0f, 0.0f, xNeg.X, xNeg.Y, xNeg.Z, xNeg.W });
	vertices.push_back({ 0.0f, 0.0f, 0.0f, xPos.X, xPos.Y, xPos.Z, xPos.W });
	vertices.push_back({ axisLength, 0.0f, 0.0f, xPos.X, xPos.Y, xPos.Z, xPos.W });

	// Y axis
	vertices.push_back({ 0.0f, 0.0f, 0.0f, yNeg.X, yNeg.Y, yNeg.Z, yNeg.W });
	vertices.push_back({ 0.0f, 0.0f, -axisLength, yNeg.X, yNeg.Y, yNeg.Z, yNeg.W });
	vertices.push_back({ 0.0f, 0.0f, 0.0f, yPos.X, yPos.Y, yPos.Z, yPos.W });
	vertices.push_back({ 0.0f,  0.0f, axisLength, yPos.X, yPos.Y, yPos.Z, yPos.W });

	// Z axis
	vertices.push_back({ 0.0f, 0.0f, 0.0f, zNeg.X, zNeg.Y, zNeg.Z, zNeg.W });
	vertices.push_back({ 0.0f, -axisLength, 0.0f, zNeg.X, zNeg.Y, zNeg.Z, zNeg.W });
	vertices.push_back({ 0.0f, 0.0f, 0.0f, zPos.X, zPos.Y, zPos.Z, zPos.W });
	vertices.push_back({ 0.0f, axisLength, 0.0f, zPos.X, zPos.Y, zPos.Z, zPos.W });

	FVertexPosColor::ChangeAxis(vertices.data(), (int32)vertices.size(), 1, 2);

	return vertices;
}
