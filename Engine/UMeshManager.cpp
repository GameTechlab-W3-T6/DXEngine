#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"
#include "Sphere.h"
#include "PlaneVertices.h"
#include "CubeVertices.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UMeshManager, UEngineSubsystem)
UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, primitiveType);
	return mesh;
}

UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosUV>& vertices,
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
	meshes["Sphere"] = CreateMeshInternal(sphere_vertices);
	//meshes["Plane"] = CreateMeshInternal(plane_vertices);
 	meshes["Plane"] = CreateMeshInternal(plane_vertices2);
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

UMesh* UMeshManager::RetrieveMesh(FString meshName)
{
	auto itr = meshes.find(meshName);
	if (itr == meshes.end()) return nullptr;
	return itr->second;
}
