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
private:
	TMap<FString, UMesh*> meshes;

	UMesh* CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UMesh* CreateWireframeMeshInternal(const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* renderer);
	UMesh* RetrieveMesh(FString meshName);
};
