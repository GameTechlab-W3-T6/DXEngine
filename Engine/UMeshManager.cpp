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
UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, primitiveType);
	return mesh;
}

// 생성자
UMeshManager::UMeshManager()
{
	// Sphere needs winding order flip for LH coordinate system
	meshes["Sphere"] = CreateMeshInternal(FlipTriangleWinding(sphere_vertices));
	meshes["Plane"] = CreateMeshInternal(plane_vertices);
	meshes["Cube"] = CreateMeshInternal(cube_vertices);
	meshes["GizmoGrid"] = CreateMeshInternal(GridGenerator::CreateGridVertices(1, 100), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	meshes["GizmoArrow"] = CreateMeshInternal(gizmo_arrow_vertices);
	meshes["GizmoRotationHandle"] = CreateMeshInternal(GridGenerator::CreateRotationHandleVertices());
	meshes["GizmoScaleHandle"] = CreateMeshInternal(gizmo_scale_handle_vertices);
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
