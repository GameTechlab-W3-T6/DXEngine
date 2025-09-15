// UMesh.h
#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "Vector4.h"

class UMesh : public UObject
{
	DECLARE_UCLASS(UMesh, UObject)
public:
	using MeshID = int32;

	ID3D11Buffer* VertexBuffer = nullptr;
	TArray<FVertexPosUV4> Vertices; // UV 좌표를 포함한 통일된 버텍스 포맷

	int32 NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = sizeof(FVertexPosUV4);

	// TODO : code review - IndexBuffer 슬쩍 추가
	ID3D11Buffer* IndexBuffer = nullptr;
	TArray<uint32> Indices;
	int32 NumIndices = 0;

	UMesh();
	// 생성자에서 초기화 리스트와 버텍스 버퍼를 생성
	UMesh(MeshID ID, const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // TODO : manual 
	// FVertexPosUV4로 통일된 생성자
	UMesh(const TArray<FVertexPosUV4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}

	void Bind(ID3D11DeviceContext* DeviceContext)
	{
		UINT Offset = 0;

		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

		DeviceContext->IASetPrimitiveTopology(PrimitiveType);

		if (IndexBuffer)
		{
			DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		}
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return isInitialized; }

	bool IsIndexBufferEnabled() const { return IndexBuffer;  }

	MeshID GetID() const
	{
		assert(ID && "ID is not initialized");
		return *ID;
	}

private:
	bool isInitialized = false;

	TOptional<MeshID> ID;
};
