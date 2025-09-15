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

//void UTextholderComp::RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled)
//{
//	if (!mesh || !mesh->VertexBuffer) return; 
//	// atlas  구별 임시 
//	if (camera == nullptr) return;
//
//	float totalWidth = textInfo->orderOfChar.size() * textInfo->cellWidth * 0.01f;
//	float penX = -totalWidth * 0.5f;
//	textInfo->center = totalWidth * 0.5f;
//
//	FMatrix view = camera->GetView();
//	FMatrix bill = textInfo->MakeBillboard(view);
//
//	for (int i = 0; i < textInfo->orderOfChar.size(); i++)
//	{
//		//addobject		
//		FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;
//		renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
//		textInfo->keyCode = textInfo->orderOfChar[i];
//		renderer.SetTextUV(*textInfo, true, bIsShaderReflectionEnabled);
//
//		renderer.DrawMesh(mesh);
//
//		penX += textInfo->cellWidth * 0.01f;
//	}
//} 
 

void UTextholderComp::RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer) return; 
	// atlas  구별 임시 
	if (camera == nullptr) return;

	TArray<FTextInstance> instances; 
	instances.reserve(textInfo->orderOfChar.size());
	 
	float totalWidth = textInfo->orderOfChar.size() * textInfo->cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	textInfo->center = totalWidth * 0.5f;

	FMatrix view = camera->GetView();
	FMatrix bill = textInfo->MakeBillboard(view);

	for (int i = 0; i < textInfo->orderOfChar.size(); i++)
	{

		const int code = (int)textInfo->orderOfChar[i];

		// (a) 인스턴스 월드행렬: translation(penX,0,0) * billboard
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;

		FTextInstance inst{}; 
		Build3x4Rows(M, inst.M0, inst.M1, inst.M2);

		// (b) UV 사각형: 아틀라스에서 code로 계산
		const int texW = textInfo->textTexture->width;
		const int texH = textInfo->textTexture->height;
		ComputeGlyphUVRect(
			code,
			(int)textInfo->cellsPerRow,
			(int)textInfo->cellsPerColumn,
			(int)textInfo->cellWidth,
			(int)textInfo->cellHeight,
			texW, texH,
			inst.uvOffset, inst.uvScale
		);

		// (c) 색상
		inst.color[0] = Color.X; inst.color[1] = Color.Y;
		inst.color[2] = Color.Z; inst.color[3] = Color.W; 
		instances.push_back(inst);

		// (d) 커서 전진
		penX += textInfo->cellWidth * 0.01f;  
	}
	renderer.DrawInstanced(mesh, instances);
}

void UTextholderComp::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{

	if (!mesh || !mesh->VertexBuffer) return;
 
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();

	renderer.SetViewProj(
		camera->GetView(), camera->GetProj()); 
	renderer.SetModel(FMatrix::Identity, Color, /*selected=*/false, /*shaderReflection=*/false);
	renderer.SetTextUV(*textInfo, bUseTextTexture, bIsShaderReflectionEnabled); 

	//UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled); 
	////UpdateConstantBuffer(renderer);

	RenderTextLine(renderer, bIsShaderReflectionEnabled);
}

//void UTextholderComp::UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
//{ 
//	FMatrix M = GetWorldTransform();
//	renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
//}
