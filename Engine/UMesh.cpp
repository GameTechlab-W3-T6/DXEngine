#include "stdafx.h"
#include "UMesh.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UMesh, UObject)

UMesh::UMesh()
{
}

UMesh::UMesh(MeshID ID, const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
	: ID(ID), Vertices(vertices), PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexPosColor4))
{
}

void UMesh::Init(ID3D11Device* device) {
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(FVertexPosUV4) * NumVertices; // Fix: 정확한 버텍스 크기 사용
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = Vertices.data();
	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create vertex buffer for mesh");
	}

	if (Indices.size() > 0)
	{
		NumIndices = Indices.size();
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.ByteWidth = static_cast<UINT>(Indices.size() * sizeof(uint32)); // Fix: 정확한 인덱스 버퍼 크기 계산
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferSRD = {};
		indexBufferSRD.pSysMem = Indices.data();

		hr = device->CreateBuffer(&indexBufferDesc, &indexBufferSRD, &IndexBuffer);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create index buffer for mesh");
		}
	}
	
	isInitialized = true;
}