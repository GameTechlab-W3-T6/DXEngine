// UMesh.h
#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "Vector4.h"

struct FVertexPosColor4; // 전방 선언

class UMesh : public UObject
{
	DECLARE_UCLASS(UMesh, UObject)
public:
	using MeshID = int32;

	ID3D11Buffer* VertexBuffer = nullptr;
	TArray<FVertexPosColor4> Vertices;
	int32 NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = 0;

	// TODO : code review - IndexBuffer 슬쩍 추가
	ID3D11Buffer* IndexBuffer = nullptr;
	TArray<uint32> Indices;
	int32 NumIndices = 0;

	UMesh();
	// 생성자에서 초기화 리스트와 버텍스 버퍼를 생성
	UMesh(MeshID ID, const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}

	TOptional<MeshID> GetID() const
	{
		return ID;
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return isInitialized; }

private:
	bool isInitialized = false;

	TOptional<MeshID> ID;
};
