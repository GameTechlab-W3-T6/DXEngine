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

void UTextholderComp::Initialize()
{
	if (isInitailized) return;

	isInitailized = true;

	if (textureManager)
	{
		FTexture* textTex = textureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		if (textTex)
		{
			TextInfo.SetParam(textTex, 16, 16);
		}
	}
}

void UTextholderComp::SetText(const FString& textContent)
{
	// TODO : 이딴 식으로 editable 핸들링 하지 말기;;
	if (isEditable)
	{
		isEditable = false;
		Initialize();
	}

	TextInfo.orderOfChar.clear();
	for (char c : textContent)
	{
		TextInfo.orderOfChar.push_back(static_cast<int>(c));
	}
}

void UTextholderComp::SetText(int32 InNumber) // TODO : hex mode 넣어 말아?
{
	FString Converted = std::to_string(InNumber);
	SetText(Converted); // 위 함수 재사용
}

// 정적 타이핑 draw용 : primitive가 자신의 textholder를 call 할 때
void UTextholderComp::DrawAboveParent(FVector location)
{
	SetPosition(location);
	Draw(*renderer, true, true);
}

void UTextholderComp::UpdateConstantBuffer(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
	renderer.SetTextUV(TextInfo, bUseTextTexture, bIsShaderReflectionEnabled);
}

// 동적 타이핑 draw용 : scene에서 draw할 때
void UTextholderComp::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{
	Initialize();
	
	// TODO : delegate로 고치기 -> editable을 on off
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();

	UpdateConstantBuffer(renderer, true, bIsShaderReflectionEnabled);
	RenderTextLine(renderer, bIsShaderReflectionEnabled);
}

void UTextholderComp::RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled)
{
	if (!mesh || !mesh->VertexBuffer) return;
	// atlas  구별 임시 
	if (camera == nullptr) return;

	float totalWidth = TextInfo.orderOfChar.size() * TextInfo.cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	TextInfo.center = totalWidth * 0.5f;

	FMatrix view = camera->GetView();
	FMatrix bill = TextInfo.MakeBillboard(view);

	for (int i = 0; i < TextInfo.orderOfChar.size(); i++)
	{
		//addobject
		TextInfo.keyCode = TextInfo.orderOfChar[i];
		renderer.SetTextUV(TextInfo, true, bIsShaderReflectionEnabled);

		if (isEditable)
		{
			 FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;
			 renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
		}
		else
		{
			FVector p = RelativeLocation;
			FMatrix M = FMatrix::TranslationRow(p.X + penX, p.Y, p.Z) * bill;
			// renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);
		}
		
		renderer.DrawMesh(mesh);

		penX += TextInfo.cellWidth * 0.01f;
	}
}

// TODO : delegate... 필요
void UTextholderComp::CaptureTypedChars()
{
	if (!isEditable ||!textureManager || !inputManager) return;

	// A~Z 키 검사 (이번 프레임 "막 눌린" 키만 받음)
	for (int vk = 'A'; vk <= 'Z'; ++vk)
	{
		if (inputManager->IsKeyPressed(vk))
		{
			if (!TextInfo.textTexture)
			{
				OutputDebugStringA("TextInfo Error:\n");
			}
			else
			{
				TextInfo.orderOfChar.push_back(vk);
			}
		}
	}
	if (inputManager->IsKeyPressed('\b'))
	{
		if (!TextInfo.orderOfChar.empty())
		{
			TextInfo.orderOfChar.pop_back();
		}
	}
}
