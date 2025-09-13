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
		
		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		//TODO: 정리
		textInfo = new FTextInfo();
		textInfo->textTexture = textTex; 
		textInfo->cellsPerColumn = 16;
		textInfo->cellsPerRow = 16;
		textInfo->cellWidth = textTex->width / textInfo->cellsPerRow;
		textInfo->cellHeight = textTex->height / textInfo->cellsPerColumn;
		  
		return mesh != nullptr;
	}

	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		return mesh != nullptr;
	}

	return false;
}
 

void UPrimitiveComponent::CaptureTypedChars()
{
	if (!textureManager || !inputManager) return;

	// A~Z 키 검사 (이번 프레임 "막 눌린" 키만 받음)
	for (int vk = 'A'; vk <= 'Z'; ++vk)
	{
		if (inputManager->IsKeyPressed(vk))
		{
			// 글자 하나당 새 FTextInfo 생성
			FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
			if (!textTex) continue;

			if (textInfo == nullptr)
			{
				textInfo = new FTextInfo();

				textInfo->textTexture = textTex;
				textInfo->cellsPerColumn = 16;
				textInfo->cellsPerRow = 16;
				textInfo->cellWidth = textTex->width / textInfo->cellsPerRow;
				textInfo->cellHeight = textTex->height / textInfo->cellsPerColumn;
				 
				textInfo->orderOfChar.push_back(vk);
			} 

			else
			{  
				textInfo->orderOfChar.push_back(vk);  
			}
		}
	}
}

void UPrimitiveComponent::RenderTextLine(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer) return;

	float penX = 0.0f; // 왼→오 펜 위치 

	for(int i = 0; i < textInfo->orderOfChar.size(); i++)
	{
		//addobject
		textInfo->keyCode = textInfo->orderOfChar[i];
		renderer.SetTextUV(*textInfo); 
		 
		//FMatrix M = GetWorldTransform(); 
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0) ;
		//FMatrix::TranslationRow()
		renderer.SetModel(M, Color, bIsSelected); 
		renderer.DrawMesh(mesh);
		 

		penX += textInfo->cellWidth * 0.01f;
	}
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

//void UPrimitiveComponent::Draw(URenderer& renderer)
//{
//	if (inputManager->IsKeyDown('Z'))
//	{
//		if (textInfo)
//		{
//			textInfo->CallCharacter((int)U'Z');
//		}
//	} 
//	if (!mesh || !mesh->VertexBuffer)
//	{
//		return;
//	}
//	 
//	UpdateConstantBuffer(renderer);
//
//	for (int i = 0; i < stringList.size(); i++)
//	{
//		renderer.SetTextUV(*stringList[i]);
//	}
//	renderer.DrawMesh(mesh);
//}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();

	if (!mesh || !mesh->VertexBuffer) return;

	//UpdateConstantBuffer(renderer);

	RenderTextLine(renderer);
}