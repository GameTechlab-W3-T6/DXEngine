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
#include "AActor.h"

IMPLEMENT_UCLASS(UTextholderComp, UPrimitiveComponent)
UCLASS_META(UTextholderComp, DisplayName, "Textholder")
UCLASS_META(UTextholderComp, MeshName, "Plane")
UCLASS_META(UTextholderComp, TextInfo, "TextInfo");
UCLASS_META(UTextholderComp, VertexShaderName, "Text_VS")
UCLASS_META(UTextholderComp, PixelShaderName, "Text_PS");

uint32 UTextholderComp::TextHolderID = 0;
bool UTextholderComp::Initialize()
{
	if (!UPrimitiveComponent::Initialize()) return false;

	cachedTextureManager = UEngineStatics::GetSubsystem<UTextureManager>();
	cachedInputManager = UEngineStatics::GetSubsystem<UInputManager>();

	float scaleNumber = 1.0f;
	SetScale({ scaleNumber, scaleNumber, scaleNumber });
	
	if (cachedTextureManager && cachedInputManager)
	{
		FTexture* textTex = cachedTextureManager->RetrieveTexture(GetClass()->GetMeta("TextInfo"));
		if (textTex)
		{
			// 16 x 16 is only fit for "TextInfo" texture(fontBlack.png, font.png)
			TextInfo.SetParam(textTex, 16, 16);
		}

		return true;
	}
	return false;
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

// ====================================================== //

void UTextholderComp::UpdateConstantBuffer(URenderer& renderer)
{
	// Calculate independent transform without parent's rotation/scale influence
	FVector worldPosition = parentTransform ?
		parentTransform->GetWorldLocation() + FVector(0.0f, 0.0f, 1.0f) :
		GetWorldLocation();

	// Create independent transform matrix with billboard rotation and fixed scale
	FMatrix independentTransform = FMatrix::SRTRowQuaternion(
		worldPosition,
		billboardRotation.ToMatrixRow(),
		FVector(1.0f, 1.0f, 1.0f)  // Fixed scale regardless of parent
	);

	FMatrix MVP = independentTransform * renderer.GetViewProj();
	(*vertexShader)["ConstantBuffer"]["MVP"] = MVP;
	(*vertexShader)["ConstantBuffer"]["MeshColor"] = Color;
	vertexShader->BindConstantBuffer(renderer.GetDeviceContext(), "ConstantBuffer");
}

void UTextholderComp::BindVertexShader(URenderer& renderer)
{
	//renderer.GetDeviceContext()->IASetInputLayout(vertexShader->GetInputLayout());
	vertexShader->Bind(renderer.GetDeviceContext(), "ConstantBuffer");
}

void UTextholderComp::BindPixelShader(URenderer& renderer)
{
	UPrimitiveComponent::BindPixelShader(renderer);
}

void UTextholderComp::Update(float deltaTime)
{
	// Call parent update first
	Super::Update(deltaTime);

	// Position will be handled in UpdateConstantBuffer
	// No need to modify scale or rotation here since we're using independent transform
}

void UTextholderComp::Draw(URenderer& renderer)
{
    USceneManager* sceneManager = UEngineStatics::GetSubsystem<USceneManager>();
    UCamera* camera = sceneManager->GetScene()->GetCamera();

    // View^-1 = Camera World Transform
    FMatrix view = camera->GetView();
    FMatrix viewInverse = FMatrix::Inverse(view);

    // Extract rotation only (zero out translation)
    FMatrix billboardRotation = viewInverse;
    billboardRotation.M[0][3] = 0.0f;
    billboardRotation.M[1][3] = 0.0f;
    billboardRotation.M[2][3] = 0.0f;
    billboardRotation.M[3][0] = 0.0f;
    billboardRotation.M[3][1] = 0.0f;
    billboardRotation.M[3][2] = 0.0f;
    billboardRotation.M[3][3] = 1.0f;

    // Convert to quaternion and store for use in UpdateConstantBuffer
    this->billboardRotation = FQuaternion::FromMatrixRow(billboardRotation);

    CreateInstanceData();
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
	//TArray<FTextInstance> instances;
	instances.reserve(TextInfo.orderOfChar.size());

	// 텍스트 전체 너비 계산 및 중앙 정렬용 penX 초기화
	float totalWidth = TextInfo.orderOfChar.size() * TextInfo.cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	TextInfo.center = totalWidth * 0.5f;


	// Create character instances with simple translation
	for (int i = 0; i < TextInfo.orderOfChar.size(); i++)
	{
		TextInfo.keyCode = TextInfo.orderOfChar[i];

		FTextInstance inst{};
		// Simple translation matrix for character positioning
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0);
		Build3x4Rows(M, inst.M0, inst.M1, inst.M2);

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
