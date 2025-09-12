#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UTextureManager.h"
#include "FTextInfo.h"
#include "FTexture.h"
#include "UInputManager.h"

//#include <fstream>
//#include <sstream>

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
UCLASS_META(UPrimitiveComponent, TextInfo, "TextInfo");

bool UPrimitiveComponent::Init(UMeshManager* meshManager, UInputManager* in, UTextureManager* tm)
{
	inputManager = in;
	textureManager = tm;

	if (textureManager && meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		texture = textureManager->RetrieveTexture(GetClass()->GetMeta("TextureType")); 
		
		//TODO: 정리
		textInfo = new FTextInfo();
		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		textInfo->textTexture = textTex; 
		textInfo->cellsPerColumn = 16;
		textInfo->cellsPerRow = 16;
		textInfo->cellWidth = textTex->width / textInfo->cellsPerRow;
		textInfo->cellHeight = textTex->height / textInfo->cellsPerColumn;
		
		stringList.push_back(textInfo);

		
		//txt 파일 읽어오기 
		//std::ifstream in(".\\Textures\\font.txt");
		//
		//if (!in.is_open())
		//{
		//	std::cerr << "Cant open font.txt file";
		//	return 0;
		//}
		//std::string line;
		//while (std::getline(in, line))
		//{
		//	if (line.empty()) continue;
		//
		//	std::istringstream iss(line);
		//	int index, value;
		//
		//	if (iss >> index >> value)
		//	{
		//		textInfo->orderOfChar.push_back(value);
		//	}
		//}

		//calculate place
		int orderH = (int)U'a' / (int)textInfo->cellsPerRow;
		int orderW = (int)U'a' % (int)textInfo->cellsPerRow;
		
		textInfo->u = (orderW * textInfo->cellWidth / textTex->width);
		textInfo->v = (orderH * textInfo->cellHeight / textTex->height);

		return mesh != nullptr;
	}

	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		return mesh != nullptr;
	}

	return false;
}

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected);

	if (textInfo != nullptr)
	{
		renderer.SetTextUV(*textInfo);
	}
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (inputManager->IsKeyDown('Z'))
	{
		textureManager->CallCharacter(stringList, GetClass(), (int)U'Z');
	
	} 
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}
	 
	UpdateConstantBuffer(renderer);

	for (int i = 0; i < stringList.size(); i++)
	{
		renderer.SetTextUV(*stringList[i]);
	}
	renderer.DrawMesh(mesh);
}
