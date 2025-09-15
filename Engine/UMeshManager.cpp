#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"
#include "Sphere.h"
#include "PlaneVertices.h"
#include "CubeVertices.h"
#include "UClass.h"

// Triangle winding order flip utility function
TArray<FVertexPosColor> FlipTriangleWinding(const TArray<FVertexPosColor>& vertices)
{
	TArray<FVertexPosColor> flipped;
	flipped.reserve(vertices.size());
	
	// Process every 3 vertices (triangle) and flip the order
	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		if (i + 2 < vertices.size())
		{
			// Flip triangle: ABC -> ACB
			flipped.push_back(vertices[i]);     // A
			flipped.push_back(vertices[i + 2]); // C  
			flipped.push_back(vertices[i + 1]); // B
		}
	}
	
	return flipped;
}

IMPLEMENT_UCLASS(UMeshManager, UEngineSubsystem)
UMesh* UMeshManager::CreateMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	UMesh* mesh = new UMesh(ID, convertedVertices, primitiveType);
	return mesh;
}

static inline uint64_t MakeEdgeKey(uint32_t a, uint32_t b)
{
	if (a > b) std::swap(a, b);                 // 무방향 키(작은,큰)
	return (uint64_t(a) << 32) | uint64_t(b);
}

UMesh* UMeshManager::CreateWireframeMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	const size_t v = vertices.size();
	if (v % 3 != 0) {
		// 로깅/예외 처리
	}
	const size_t triCount = v / 3;

	TSet<uint64_t> unique;
	unique.reserve(triCount * 3);

	TArray<uint32> lineIdx;
	lineIdx.reserve(triCount * 6);              // 라인 인덱스는 엣지당 2개

	auto AddEdge = [&](uint32_t a, uint32_t b)
		{
			const uint64_t k = MakeEdgeKey(a, b);
			if (unique.emplace(k).second) {         // 새로 추가된 경우만 push
				lineIdx.push_back(a);
				lineIdx.push_back(b);
			}
		};

	for (uint32_t i = 0; i < v; i += 3) {
		AddEdge(i, i + 1);
		AddEdge(i + 1, i + 2);
		AddEdge(i + 2, i);
	}
	
	// 정점 포맷 변환(필요시)
	TArray<FVertexPosColor4> converted = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());

	// 메시 생성: 토폴로지는 반드시 LINELIST
	UMesh* mesh = new UMesh(ID, converted, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	mesh->Indices = lineIdx;
	mesh->NumIndices = lineIdx.size();
	
	return mesh;
}

// 생성자
UMeshManager::UMeshManager()
{
	// Sphere needs winding order flip for LH coordinate system
	// meshes["Sphere"] = CreateMeshInternal(FlipTriangleWinding(sphere_vertices));
	meshes["Sphere"] = CreateMeshInternal(GetNextID(), FlipTriangleWinding(sphere_vertices));
	meshes["Plane"] = CreateMeshInternal(GetNextID(), plane_vertices);
	meshes["Cube"] = CreateMeshInternal(GetNextID(), cube_vertices);

	meshes["GizmoGrid"] = CreateMeshInternal(GetNextID(), GridGenerator::CreateGridVertices(1, 100), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	meshes["GizmoArrow"] = CreateMeshInternal(GetNextID(), gizmo_arrow_vertices);
	meshes["GizmoRotationHandle"] = CreateMeshInternal(GetNextID(), GridGenerator::CreateRotationHandleVertices());
	meshes["GizmoScaleHandle"] = CreateMeshInternal(GetNextID(), gizmo_scale_handle_vertices);

	// meshes["SphereWireframe"] = CreateWireframeMeshInternal(FlipTriangleWinding(sphere_vertices), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

// 소멸자 (메모리 해제)
UMeshManager::~UMeshManager()
{
	for (auto& pair : meshes)
	{
		delete pair.second;
	}
	meshes.clear();
}

bool UMeshManager::Initialize(URenderer* renderer)
{
	if (!renderer) return false;

	try
	{
		for (const auto& var : meshes)
		{
			var.second->Init(renderer->GetDevice());
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "UMeshManager::Initialize failed: " << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "UMeshManager::Initialize failed: unknown exception" << std::endl;
		return false;
	}

	return true;
}

UMesh* UMeshManager::RetrieveMesh(FString meshName)
{
	auto itr = meshes.find(meshName);
	if (itr == meshes.end()) return nullptr;
	return itr->second;
}
