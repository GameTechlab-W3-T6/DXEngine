#pragma once

#include "UMesh.h"
#include "URenderer.h"
#include "UEngineSubsystem.h"

/**
 * @brief Mesh resource manager handling mesh creation and retrieval
 */
class UMeshManager : public UEngineSubsystem
{
	DECLARE_UCLASS(UMeshManager, UEngineSubsystem)
public:
	using MeshID = UMesh::MeshID;

private:
	MeshID GetNextID() const
	{
		return meshes.size();
	}

	TMap<FString, TUniquePtr<UMesh>> meshes;

	TUniquePtr<UMesh> CreateMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	TUniquePtr<UMesh> CreateWireframeMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

public:
	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* renderer);
	UMesh* GetMesh(FString meshName)
	{
		auto it = meshes.find(meshName);

		if (it != meshes.end())
		{
			return it->second.get();
		}

		return nullptr;
	}
};
