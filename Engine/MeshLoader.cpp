#include "stdafx.h"

#include <sstream>
#include <random>
#include <tuple>
#include <map>

#include "MeshLoader.h"

#include "FVertexPosColor.h"

template<>
std::pair<TArray<FVertexPosColor>, TArray<UINT>> MeshLoader::LoadMeshWithIndex(const std::filesystem::path& FilePath)
{
	LoadMeshImpl(FilePath);

	TArray<FVertexPosColor> VertexArray;
	TArray<UINT> IndexArray;
	/** @todo: Change this into TMap after implementing hashing algorithm */
	std::map<std::tuple<int32, int32, int32>, UINT> IndexMap;
	for (const auto& Face : Faces)
	{
		auto FaceKey = std::make_tuple(Face.PositionIndex, Face.NormalIndex, Face.TexCoordIndex);

		if (IndexMap.find(FaceKey) != IndexMap.end())
		{
			IndexArray.push_back(IndexMap[FaceKey]);
		}
		else
		{
			IndexMap[FaceKey] = VertexArray.size();
			FVertexPosColor Vertex;
			if (Face.PositionIndex != -1)
			{
				FVector Position = Positions[Face.PositionIndex];
				Vertex.x = Position.X;
				Vertex.y = Position.Y;
				Vertex.z = Position.Z;
			}
			if (Face.NormalIndex != -1)
			{
				FVector Normal = Normals[Face.NormalIndex];
				// DO NOTHING
			}
			if (Face.TexCoordIndex != -1)
			{
				FVector2 TexCoord = TexCoords[Face.TexCoordIndex];
				// DO NOTHING
			}
			/* COLOR */
			std::random_device RandomDevice;
			std::mt19937 Gen(RandomDevice());
			std::uniform_real_distribution<float> Dist(0.0f, 1.0f);
			Vertex.r = Dist(Gen);
			Vertex.g = Dist(Gen);
			Vertex.b = Dist(Gen);
			Vertex.a = 1.0f;

			VertexArray.push_back(Vertex);
			IndexArray.push_back(IndexMap[FaceKey]);
		}
	}
	return { VertexArray, IndexArray };
}

template<>
std::pair<TArray<FVertexPosColorUV4>, TArray<UINT>> MeshLoader::LoadMeshWithIndex(const std::filesystem::path& FilePath)
{
	LoadMeshImpl(FilePath);

	TArray<FVertexPosColorUV4> VertexArray;
	TArray<UINT> IndexArray;
	/** @todo: Change this into TMap after implementing hashing algorithm */
	std::map<std::tuple<int32, int32, int32>, UINT> IndexMap;
	for (const auto& Face : Faces)
	{
		auto FaceKey = std::make_tuple(Face.PositionIndex, Face.NormalIndex, Face.TexCoordIndex);

		if (IndexMap.find(FaceKey) != IndexMap.end())
		{
			IndexArray.push_back(IndexMap[FaceKey]);
		}
		else
		{
			IndexMap[FaceKey] = VertexArray.size();
			FVertexPosColorUV4 Vertex;
			if (Face.PositionIndex != -1)
			{
				FVector Position = Positions[Face.PositionIndex];
				Vertex.x = Position.X;
				Vertex.y = Position.Y;
				Vertex.z = Position.Z;
			}
			if (Face.NormalIndex != -1)
			{
				FVector Normal = Normals[Face.NormalIndex];
				// DO NOTHING
			}
			if (Face.TexCoordIndex != -1)
			{
				FVector2 TexCoord = TexCoords[Face.TexCoordIndex];
				Vertex.u = TexCoord.X;
				Vertex.v = TexCoord.Y;
			}
			/* COLOR */
			std::random_device RandomDevice;
			std::mt19937 Gen(RandomDevice());
			std::uniform_real_distribution<float> Dist(0.0f, 1.0f);
			Vertex.r = Dist(Gen);
			Vertex.g = Dist(Gen);
			Vertex.b = Dist(Gen);
			Vertex.a = 1.0f;

			VertexArray.push_back(Vertex);
			IndexArray.push_back(IndexMap[FaceKey]);
		}
	}
	return { VertexArray, IndexArray };
}

template<>
TArray<FVertexPosColorUV4> MeshLoader::LoadMesh(const std::filesystem::path& FilePath)
{
	LoadMeshImpl(FilePath);

	TArray<FVertexPosColorUV4> VertexArray;
	/** @todo: Change this into TMap after implementing hashing algorithm */
	for (const auto& Face : Faces)
	{
		FVertexPosColorUV4 Vertex;
		if (Face.PositionIndex != -1)
		{
			FVector Position = Positions[Face.PositionIndex];
			Vertex.x = Position.X;
			Vertex.y = Position.Y;
			Vertex.z = Position.Z;
		}
		if (Face.NormalIndex != -1)
		{
			FVector Normal = Normals[Face.NormalIndex];
			// DO NOTHING
		}
		if (Face.TexCoordIndex != -1)
		{
			FVector2 TexCoord = TexCoords[Face.TexCoordIndex];
			Vertex.u = TexCoord.X;
			Vertex.v = TexCoord.Y;
		}
		/* COLOR */
		std::random_device RandomDevice;
		std::mt19937 Gen(RandomDevice());
		std::uniform_real_distribution<float> Dist(0.0f, 1.0f);
		Vertex.r = Dist(Gen);
		Vertex.g = Dist(Gen);
		Vertex.b = Dist(Gen);
		Vertex.a = 1.0f;

		VertexArray.push_back(Vertex);
	}
	return VertexArray;
}

void MeshLoader::LoadMeshImpl(const std::filesystem::path& FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		assert(false); /** @todo */
	}

	std::ifstream File(FilePath);
	if (!File)
	{
		assert(false); /** @todo */
	}

	Positions.clear();
	Normals.clear();
	TexCoords.clear();
	Faces.clear();

	FString Line;
	while (std::getline(File, Line))
	{
		std::istringstream Tokenizer(Line);

		FString Prefix;
		Tokenizer >> Prefix;

		if (Prefix == "v")
		{
			FVector Position;
			Tokenizer >> Position.X >> Position.Y >> Position.Z;
			Positions.push_back(Position);
		}
		else if (Prefix == "vn")
		{
			FVector Normal;
			Tokenizer >> Normal.X >> Normal.Y >> Normal.Z;
			Normals.push_back(Normal);
		}
		else if (Prefix == "vt")
		{
			FVector2 TexCoord;
			Tokenizer >> TexCoord.X >> TexCoord.Y;
			TexCoords.push_back(TexCoord);
		}
		else if (Prefix == "f")
		{
			FString FaceBuffer;
			while (Tokenizer >> FaceBuffer)
			{
				Faces.push_back(ParseFaceBuffer(FaceBuffer));
			}
		}
	}
}

MeshLoader::FFace MeshLoader::ParseFaceBuffer(const FString& FaceBuffer)
{
	FFace Face = {};
	std::istringstream Tokenizer(FaceBuffer);
	FString IndexBuffer;

	std::getline(Tokenizer, IndexBuffer, '/');

	Face.PositionIndex = std::stoi(IndexBuffer) - 1;

	if (std::getline(Tokenizer, IndexBuffer, '/') && !IndexBuffer.empty())
	{
		Face.TexCoordIndex = std::stoi(IndexBuffer) - 1;
	}

	if (std::getline(Tokenizer, IndexBuffer, '/') && !IndexBuffer.empty())
	{
		Face.NormalIndex = std::stoi(IndexBuffer) - 1;
	}

	return Face;
}
