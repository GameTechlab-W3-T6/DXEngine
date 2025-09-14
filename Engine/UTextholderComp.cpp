#include "stdafx.h"
#include "UClass.h"
#include "UCamera.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "UTextholderComp.h"
#include "UInputManager.h"
#include "UMeshManager.h"
#include "UTextureManager.h"
IMPLEMENT_UCLASS(UTextholderComp, UPrimitiveComponent)
UCLASS_META(UTextholderComp, DisplayName, "Textholder")
UCLASS_META(UTextholderComp, MeshName, "Plane")
UCLASS_META(UTextholderComp, TextInfo, "TextInfo");

bool UTextholderComp::Init(UMeshManager* meshManager, UInputManager* im, UTextureManager* tm, UCamera* cam)
{
	inputManager = im;
	textureManager = tm;

	if (textureManager && meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
		texture = tm->RetrieveTexture(GetClass()->GetMeta("TextureType"));

		FTexture* textTex = tm->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		textInfo->SetParam(textTex, 16, 16);
		camera = cam;

		return mesh != nullptr;
	}

	return true;
}

void UTextholderComp::CaptureTypedChars()
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

void UTextholderComp::RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer) return; 
	// atlas  구별 임시 
	if (camera == nullptr) return;

	float totalWidth = textInfo->orderOfChar.size() * textInfo->cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	textInfo->center = totalWidth * 0.5f;

	FMatrix view = camera->GetView();
	FMatrix bill = textInfo->MakeBillboard(view);

	for (int i = 0; i < textInfo->orderOfChar.size(); i++)
	{
		//addobject
		textInfo->keyCode = textInfo->orderOfChar[i];
		renderer.SetTextUV(*textInfo, true, bIsShaderReflectionEnabled);
		 
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;
		renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
		renderer.DrawMesh(mesh);

		penX += textInfo->cellWidth * 0.01f;
	}
}

void UTextholderComp::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{

	if (!mesh || !mesh->VertexBuffer) return;
 
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();

	
	UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled);
	////UpdateConstantBuffer(renderer);

	RenderTextLine(renderer, bIsShaderReflectionEnabled);
}

//void UTextholderComp::UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
//{ 
//	FMatrix M = GetWorldTransform();
//	renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
//}
