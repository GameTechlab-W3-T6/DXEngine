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
/**
* @brief 인스턴싱을 이용해 텍스트를 렌더링합니다.
*
* 이 함수는 `textInfo->orderOfChar`에 포함된 각 문자에 대해
* 인스턴스 데이터(월드 변환 행렬, UV 좌표, 색상)를 생성하고
* 렌더러를 통해 instanced draw call을 호출합니다.
*/
void UTextholderComp::RenderTextLine(URenderer& renderer, bool bIsShaderReflectionEnabled)
{ 

	// 문자 개수만큼 인스턴스 배열 공간 확보
	TArray<FTextInstance> instances; 
	instances.reserve(textInfo->orderOfChar.size());
	 
	// 텍스트 전체 너비 계산 및 중앙 정렬용 penX 초기화
	float totalWidth = textInfo->orderOfChar.size() * textInfo->cellWidth * 0.01f;
	float penX = -totalWidth * 0.5f;
	textInfo->center = totalWidth * 0.5f;

	// 카메라 View의 역행렬에서 Rotation 성분만 추출한 행렬을 사용해서 빌보드 행렬 생성
	FMatrix view = camera->GetView();
	FMatrix bill = textInfo->MakeBillboard(view);

	for (int i = 0; i < textInfo->orderOfChar.size(); i++)
	{ 
		const int code = (int)textInfo->orderOfChar[i];

		// 인스턴스 월드 행렬: X축으로 penX 만큼 이동 후 빌보드 적용
		FMatrix M = FMatrix::TranslationRow(penX, 0, 0) * bill;

		// 행렬을 float4 로 분리해 인스턴스 구조체에 저장
		FTextInstance inst{}; 
		Build3x4Rows(M, inst.M0, inst.M1, inst.M2);

		// 아틀라스에서 현재 글자의 UV 사각형 계산
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

		// 글자 색상지정
		inst.color[0] = Color.X; inst.color[1] = Color.Y;
		inst.color[2] = Color.Z; inst.color[3] = Color.W; 
		instances.push_back(inst);
		
		// text 가운데 정렬
		penX += textInfo->cellWidth * 0.01f;  
	}
		 
	renderer.DrawInstanced(mesh, instances);
}

void UTextholderComp::Draw(URenderer& renderer, bool bUseTextTexture, bool bIsShaderReflectionEnabled)
{

	if (!mesh || !mesh->VertexBuffer) return;
 
	// 텍스트 입력 먼저 처리 
	CaptureTypedChars();
	 
	renderer.SetTextUV(*textInfo, bUseTextTexture, bIsShaderReflectionEnabled); 

	// instaced 정보 저장 + instanced용 layout,shader로 교체해서 render
	RenderTextLine(renderer, bIsShaderReflectionEnabled);
} 