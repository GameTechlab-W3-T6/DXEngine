#include "stdafx.h"
#include "UMesh.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UMesh, UObject)

UMesh::UMesh()
{
}

UMesh::UMesh(const TArray<FVertexPosUV4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
	: Vertices(vertices), PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexPosUV4))
{
}
UMesh::UMesh(const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
	: Vertices2(vertices), PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexPosColor4))
{
} 

void UMesh::Init(ID3D11Device* device, bool isFVertexPosUV) {
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(FVertexPosColor4) * NumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {}; 
	if (isFVertexPosUV)
	{
		vertexData.pSysMem = Vertices.data();
	}
	else
	{
		vertexData.pSysMem = Vertices2.data();
    }
	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
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