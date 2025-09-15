#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"
#include "Sphere.h"
#include "PlaneVertices.h"
#include "CubeVertices.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UMeshManager, UEngineSubsystem)
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

TUniquePtr<UMesh> UMeshManager::CreateMeshInternal(const TArray<FVertexPosUV>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	TUniquePtr<UMesh> mesh = MakeUnique<UMesh>(convertedVertices, primitiveType);
	return mesh;
}

TUniquePtr<UMesh> UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	TUniquePtr<UMesh> mesh = MakeUnique<UMesh>(convertedVertices, primitiveType);
	return mesh;
}

static inline uint64_t MakeEdgeKey(uint32_t a, uint32_t b)
{
	if (a > b) std::swap(a, b);                 // 무방향 키(작은,큰)
	return (uint64_t(a) << 32) | uint64_t(b);
}


// 생성자
UMeshManager::UMeshManager()
{
	// Sphere needs winding order flip for LH coordinate system
	// meshes["Sphere"] = CreateMeshInternal(FlipTriangleWinding(sphere_vertices));
	meshes["Sphere"] = CreateMeshInternal(FlipTriangleWinding(sphere_vertices));
	meshes["Plane"] = CreateMeshInternal(plane_vertices);
	meshes["Cube"] = CreateMeshInternal(cube_vertices);

	ConfigData* config = ConfigManager::GetConfig("editor");
	//float gridSize = config->getFloat("Gizmo", "GridSize");
	int gridCount = config->getInt("Gizmo", "GridCount");
	meshes["GizmoGrid"] = CreateMeshInternal(GridGenerator::CreateGridVertices(1.0f, gridCount), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	meshes["GizmoArrow"] = CreateMeshInternal(gizmo_arrow_vertices);
	meshes["GizmoRotationHandle"] = CreateMeshInternal(GridGenerator::CreateRotationHandleVertices());
	meshes["GizmoScaleHandle"] = CreateMeshInternal(gizmo_scale_handle_vertices);

	// meshes["SphereWireframe"] = CreateWireframeMeshInternal(FlipTriangleWinding(sphere_vertices), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

// 소멸자 (메모리 해제)
UMeshManager::~UMeshManager()
{
	meshes.clear();
}

bool UMeshManager::Initialize(URenderer* renderer)
{
	if (!renderer) return false;

	try
	{
		for (const auto& var : meshes)
		{
			// Vertices(FVertexPosColor4) 데이터가 있으면 isFVertexPosColor를 true로 전달
			if (var.second->Vertices.size() > 0)
			{
				var.second->Init(renderer->GetDevice(), true);

			}
			// Vertices2(FVertexPosUV) 데이터만 있으면 isFVertexPosColor를 false로 전달
			else
			{
				var.second->Init(renderer->GetDevice(), false);
			}
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

UMesh* UMeshManager::GetMesh(FString meshName)
{
	auto itr = meshes.find(meshName);
	if (itr == meshes.end()) return nullptr;
	return itr->second.get();
}
