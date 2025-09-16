#include "stdafx.h"
#include "UMesh.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UMesh, UObject)

UMesh::UMesh()
{
}

//UMesh::UMesh(MeshID ID, const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
//	: ID(ID), Vertices(vertices), PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexPosColor4))
//{
//}

UMesh::UMesh(MeshID ID, const TArray<FVertexPosColorUV4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
	: ID(ID), Vertices(vertices), PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexPosColorUV4))
{
}

void UMesh::Init(ID3D11Device* device) {
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	//vertexBufferDesc.ByteWidth = sizeof(FVertexPosColor4) * NumVertices;
	vertexBufferDesc.ByteWidth = sizeof(FVertexPosColorUV4) * NumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferSRD = {};
	vertexBufferSRD.pSysMem = Vertices.data();

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferSRD, &VertexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create vertex buffer for mesh");
	}

	if (Indices.size() > 0)
	{
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.ByteWidth = sizeof(Indices);
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