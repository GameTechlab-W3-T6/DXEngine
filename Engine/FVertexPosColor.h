#pragma once
#include "Vector.h"
#include "TArray.h"
#include "UEngineStatics.h"

struct FVertexPosColor
{
	float x, y, z;       // Position (3개)
	float r, g, b, a;    // Color (4개)

	// 정적 함수 ChangeAxis 구현
	static void ChangeAxis(FVertexPosColor vertices[], int32 count, int32 axis1, int32 axis2)
	{
		// 유효성 검사
		if (axis1 < 0 || 2 < axis1 || axis2 < 0 || 2 < axis2 || axis1 == axis2)
		{
			// 유효하지 않은 축 인덱스인 경우 함수 종료
			return;
		}

		// 모든 버텍스를 순회하며 축 교환
		for (int32 i = 0; i < count; ++i)
		{
			//std::swap(vertices[i].y, vertices[i].z);
			float* pCoords = &vertices[i].x; // 첫 번째 좌표의 포인터
			std::swap(pCoords[axis1], pCoords[axis2]);
		}
	}
};

struct FVertexPosUV
{
	float x, y, z; 

	float u, v;

	// 정적 함수 ChangeAxis 구현
	static void ChangeAxis(FVertexPosUV vertices[], int32 count, int32 axis1, int32 axis2)
	{
		// 유효성 검사
		if (axis1 < 0 || 2 < axis1 || axis2 < 0 || 2 < axis2 || axis1 == axis2)
		{
			// 유효하지 않은 축 인덱스인 경우 함수 종료
			return;
		}

		// 모든 버텍스를 순회하며 축 교환
		for (int32 i = 0; i < count; ++i)
		{
			//std::swap(vertices[i].y, vertices[i].z);
			float* pCoords = &vertices[i].x; // 첫 번째 좌표의 포인터
			std::swap(pCoords[axis1], pCoords[axis2]);
		}
	}
};

struct FVertexPosUV4
{
	float x, y, z, w;
	float u, v;

	// 정적 함수 ChangeAxis 구현
	static void ChangeAxis(FVertexPosUV vertices[], int32 count, int32 axis1, int32 axis2)
	{
		// 유효성 검사
		if (axis1 < 0 || 2 < axis1 || axis2 < 0 || 2 < axis2 || axis1 == axis2)
		{
			// 유효하지 않은 축 인덱스인 경우 함수 종료
			return;
		}

		// 모든 버텍스를 순회하며 축 교환
		for (int32 i = 0; i < count; ++i)
		{
			//std::swap(vertices[i].y, vertices[i].z);
			float* pCoords = &vertices[i].x; // 첫 번째 좌표의 포인터
			std::swap(pCoords[axis1], pCoords[axis2]);
		}
	}
};

//정점 구조체 선언, 삼각형 배열 정의
// 1. Define the triangle vertices
struct FVertexPosColor4
{
	float x, y, z, w;       // Position (float4) - w 추가
	float r, g, b, a;       // Color (float4)

	// 변환 함수
	static TArray<FVertexPosColor4> ConvertVertexData(const FVertexPosColor* oldVertices, int32 count)
	{
		TArray<FVertexPosColor4> newVertices;
		newVertices.reserve(count);

		for (int32 i = 0; i < count; i++)
		{
			FVertexPosColor4 newVertex;

			// Position: w = 1.0f 추가
			newVertex.x = oldVertices[i].x;
			newVertex.y = oldVertices[i].y;
			newVertex.z = oldVertices[i].z;
			newVertex.w = 1.0f;  // 동차좌표 w 컴포넌트

			// Color: 그대로 복사
			newVertex.r = oldVertices[i].r;
			newVertex.g = oldVertices[i].g;
			newVertex.b = oldVertices[i].b;
			newVertex.a = oldVertices[i].a;

			newVertices.push_back(newVertex);
		}

		return newVertices;
	}

	static TArray<FVertexPosUV4> ConvertVertexData(const FVertexPosUV* oldVertices, int32 count)
	{
		TArray<FVertexPosUV4> newVertices;
		newVertices.reserve(count);

		for (int32 i = 0; i < count; i++)
		{
			FVertexPosUV4 newVertex;

			// Position: w = 1.0f 추가
			newVertex.x = oldVertices[i].x;
			newVertex.y = oldVertices[i].y;
			newVertex.z = oldVertices[i].z;
			newVertex.w = 1.0f;  // 동차좌표 w 컴포넌트

			// Color: 그대로 복사
			newVertex.u = oldVertices[i].u;
			newVertex.v = oldVertices[i].v; 

			newVertices.push_back(newVertex);
		}

		return newVertices;
	}

	FVector GetPosition()
	{
		return {x, y, z};
	}

};



