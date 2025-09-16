#pragma once
#include <filesystem>
#include "Vector.h"
#include "TArray.h"
#include "UEngineStatics.h"

class MeshLoader
{
private:
	struct FFace
	{
		int32 PositionIndex = -1;
		int32 NormalIndex = -1;
		int32 TexCoordIndex = -1;
	};

public:
	static MeshLoader& GetInstance()
	{
		static MeshLoader Instance;
		return Instance;
	}

	~MeshLoader() = default;

	MeshLoader(const MeshLoader&) = delete;
	MeshLoader(MeshLoader&&) = delete;

	MeshLoader& operator=(const MeshLoader&) = delete;
	MeshLoader& operator=(MeshLoader&&) = delete;

	template<typename TVertex>
	std::pair<TArray<TVertex>, TArray<UINT>>
		LoadMeshWithIndex(const std::filesystem::path& FilePath);

	template<typename TVertex>
	TArray<TVertex> LoadMesh(const std::filesystem::path& FilePath);

private:
	MeshLoader() = default;

	void LoadMeshImpl(const std::filesystem::path& FilePath);

	FFace ParseFaceBuffer(const FString& FaceBuffer);

	TArray<FVector> Positions;
	TArray<FVector> Normals;
	TArray<FVector2> TexCoords;
	TArray<FFace> Faces;
};

