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
UCLASS_META(UTextholderComp, TextInfo, "TextInfo"); // TODO : texture data 같은 걸로 바꾸기

void UTextholderComp::SetText(const FString& textContent)
{
	// TODO : 이딴 식으로 editable 핸들링 하지 말기;;
	if (isEditable)
	{
		isEditable = false;
	}
	// TODO : 이딴식으로 mesh 초기화하지 말기. 따로 초기화하는 방법...
	if (!mesh)
	{
		mesh = meshManager->RetrieveMesh(GetClass()->GetMeta("MeshName"));
	}

	TextInfo->orderOfChar.clear();
	for (char c : textContent)
	{
		TextInfo->orderOfChar.push_back(static_cast<int>(c));
	}
}

void UTextholderComp::SetText(int32 InNumber) // TODO : hex mode 넣어 말아?
{
	FString Converted = std::to_string(InNumber);
	SetText(Converted); // 위 함수 재사용
}

// 정적 타이핑 draw용 : primitive가 자신의 textholder를 call 할 때
void UTextholderComp::Draw(FVector location)
{
	// TODO : Set position도 draw와 분리 필요
	SetPosition(location);

	//  TODO : 제발제발제발제발 고쳐야함
	if (!mesh || !mesh->VertexBuffer) return;

	UpdateConstantBuffer(*renderer, true, false);
	RenderTextLine(*renderer, false);
}

// 동적 타이핑 draw용 : scene에서 draw할 때
void UTextholderComp::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer) return;
 
	// TODO : delegate로 고치기 
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();

	UpdateConstantBuffer(renderer, bUseTextTexture, bIsShaderReflectionEnabled);
	RenderTextLine(renderer, bIsShaderReflectionEnabled);
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

// TODO : delegate... 제발
void UTextholderComp::CaptureTypedChars()
{
	if (!isEditable || !textureManager || !inputManager) return;

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
