#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"
#include "UTextureManager.h"
#include "FTextInfo.h"
#include "FTexture.h"
#include "UInputManager.h"
#include "UCamera.h"

//#include <fstream>
//#include <sstream>

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
UCLASS_META(UPrimitiveComponent, TextInfo, "TextInfo");

bool UPrimitiveComponent::Init(UMeshManager* meshManager, UInputManager* in, UTextureManager* tm, UCamera* cam)
{
	inputManager = in;
	textureManager = tm;
	
	if (textureManager && meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		texture = textureManager->RetrieveTexture(GetClass()->GetMeta("TextureType")); 
		
		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo")); 
		textInfo->SetParam(textTex, 16, 16); 
		camera = cam;

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
			if (textInfo == nullptr)
			{  
				OutputDebugStringA("TextInfo Error:\n"); 
			}  
			else
			{  
				textInfo->orderOfChar.push_back(vk);  
			}
		}
	}
	if (inputManager->IsKeyPressed('\b'))
	{
		if (!textInfo->orderOfChar.empty())
		{
			textInfo->orderOfChar.pop_back();
		}
	}

}

void UPrimitiveComponent::RenderTextLine(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer) return;

	// atlas  구별 임시
	
	if (camera == nullptr) return;

	float totalWidth = textInfo->orderOfChar.size() * textInfo->cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	textInfo->center = totalWidth * 0.5f;
	
	FMatrix view = camera->GetView(); 
	FMatrix bill = textInfo->MakeBillboard(view); 
	for(int i = 0; i < textInfo->orderOfChar.size(); i++)
	{
		//addobject
		textInfo->keyCode = textInfo->orderOfChar[i];
		renderer.SetTextUV(*textInfo);  

		//FMatrix M = FMatrix::TranslationRow(penX, 0, 0);
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0)  * bill;
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

	renderer.DrawMesh(mesh);
}