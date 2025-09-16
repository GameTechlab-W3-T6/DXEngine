#include "stdafx.h"
#include "UClass.h"
#include "UCamera.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "UBatchShaderManager.h"
#include "UTextholderComp.h"
#include "UInputManager.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "UScene.h"
#include "UTextureManager.h"

IMPLEMENT_UCLASS(UTextholderComp, UPrimitiveComponent)
UCLASS_META(UTextholderComp, DisplayName, "Textholder")
UCLASS_META(UTextholderComp, MeshName, "Plane")
UCLASS_META(UTextholderComp, TextInfo, "TextInfo");
UCLASS_META(UTextholderComp, VertexShaderName, "Text_VS")
UCLASS_META(UTextholderComp, PixelShaderName, "Text_PS");

void UTextholderComp::ResourcesInitialize()
{
	if (isInitailized) return;

	isInitailized = true;

	cachedTextureManager = UEngineStatics::GetSubsystem<UTextureManager>();
	cachedInputManager = UEngineStatics::GetSubsystem<UInputManager>();

	if (cachedTextureManager)
	{
		FTexture* textTex = cachedTextureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		texture = textTex;
		if (textTex)
		{
			TextInfo.SetParam(textTex, 16, 16);
		}
	}
	// UBatchShaderManager* batchShaderManager = UEngineStatics::GetSubsystem<UBatchShaderManager>();
}

void UTextholderComp::SetText(const FString& textContent)
{
	// TODO : 이딴 식으로 editable 핸들링 하지 말기;;
	if (isEditable)
	{
		isEditable = false;
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
void UTextholderComp::DrawAboveParent(URenderer& renderer, FVector location)
{
	SetPosition(location);
	Draw(renderer);
}

// ====================================================== //

void UTextholderComp::UpdateConstantBuffer(URenderer& renderer)
{
	UPrimitiveComponent::UpdateConstantBuffer(renderer);
}

void UTextholderComp::BindVertexShader(URenderer& renderer)
{
	UPrimitiveComponent::BindVertexShader(renderer);
}

void UTextholderComp::BindPixelShader(URenderer& renderer)
{
	UPrimitiveComponent::BindPixelShader(renderer);
}

void UTextholderComp::Draw(URenderer& renderer)
{
	ResourcesInitialize();

	CaptureTypedChars();

	CreateInstance();

	renderer.DrawTextholderComponent(this);
}

// ====================================================== //

/**
* @brief 인스턴싱을 이용해 텍스트를 렌더링합니다.
*
* 이 함수는 `textInfo->orderOfChar`에 포함된 각 문자에 대해
* 인스턴스 데이터(월드 변환 행렬, UV 좌표, 색상)를 생성하고
* 렌더러를 통해 instanced draw call을 호출합니다.
*/
// TODO : delegate... 필요
void UTextholderComp::CaptureTypedChars()
{
	if (!isEditable || !cachedTextureManager || !cachedInputManager) return;

	// A~Z 키 검사 (이번 프레임 "막 눌린" 키만 받음)
	for (int vk = 'A'; vk <= 'Z'; ++vk)
	{
		if (cachedInputManager->IsKeyPressed(vk))
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
	if (cachedInputManager->IsKeyPressed('\b'))
	{
		if (!TextInfo.orderOfChar.empty())
		{
			TextInfo.orderOfChar.pop_back();
		}
	}
}

void UTextholderComp::CreateInstanceData()
{
	/** @note: Should clear before creation. */
	instances.clear();

	//if (!mesh || !mesh->VertexBuffer) return;
	// atlas  구별 임시 
	//if (camera == nullptr) return;

	USceneManager* sceneManager = UEngineStatics::GetSubsystem<USceneManager>();
	UCamera* camera = sceneManager->GetScene()->GetCamera();

	// 문자 개수만큼 인스턴스 배열 공간 확보
	TArray<FTextInstance> instances;
	instances.reserve(TextInfo.orderOfChar.size());

	// 텍스트 전체 너비 계산 및 중앙 정렬용 penX 초기화
	float totalWidth = TextInfo.orderOfChar.size() * TextInfo.cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	TextInfo.center = totalWidth * 0.5f;

	// 카메라 View의 역행렬에서 Rotation 성분만 추출한 행렬을 사용해서 빌보드 행렬 생성
	FMatrix view = camera->GetView();
	for (int i = 0; i < TextInfo.orderOfChar.size(); i++)
	{
		//addobject
		TextInfo.keyCode = TextInfo.orderOfChar[i];

		FTextInstance inst{};
		if (isEditable)
		{
			FMatrix bill = TextInfo.MakeBillboard(view);
			FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;
			//renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);

			// 행렬을 float4 로 분리해 인스턴스 구조체에 저장 
			Build3x4Rows(M, inst.M0, inst.M1, inst.M2);
		}
		else
		{
			FMatrix bill = TextInfo.MakeBillboard(view);
			FVector p = RelativeLocation;
			//SetPosition(p);
			UE_LOG("%.2f, %.2f, %.2f", p.X + penX, p.Y, p.Z);
			FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;
			// FMatrix bill = TextInfo.MakeBillboard(RelativeLocation, camera->GetLocation());
			// FMatrix M = FMatrix::Identity  * bill;// GetWorldTransform();
			// FMatrix M = FMatrix::TranslationRow(p.X, p.Y, p.Z);//*bill;
			// FMatrix M = FMatrix::TranslationRow(p.X + penX, p.Y, p.Z);//*bill;
			// renderer.SetModel(M, Color, bIsSelected, bIsShaderReflectionEnabled);

			// 행렬을 float4 로 분리해 인스턴스 구조체에 저장
			Build3x4Rows(M, inst.M0, inst.M1, inst.M2);
		}

		// 인스턴스 월드 행렬: X축으로 penX 만큼 이동 후 빌보드 적용


		// 아틀라스에서 현재 글자의 UV 사각형 계산
		const int texW = TextInfo.textTexture->width;
		const int texH = TextInfo.textTexture->height;
		ComputeGlyphUVRect(
			TextInfo.keyCode,
			(int)TextInfo.cellsPerRow,
			(int)TextInfo.cellsPerColumn,
			(int)TextInfo.cellWidth,
			(int)TextInfo.cellHeight,
			texW, texH,
			inst.uvOffset, inst.uvScale
		);

		// 글자 색상지정
		inst.color[0] = Color.X; inst.color[1] = Color.Y;
		inst.color[2] = Color.Z; inst.color[3] = Color.W;
		instances.push_back(inst);

		// text 가운데 정렬
		penX += TextInfo.cellWidth * 0.01f;
	}
}