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

	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* renderer);
	UMesh* RetrieveMesh(FString meshName);

private:
	MeshID GetNextID()
	{
		return meshes.size();
	}

	TMap<FString, UMesh*> meshes;

	UMesh* CreateMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UMesh* CreateWireframeMeshInternal(MeshID ID, const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};
