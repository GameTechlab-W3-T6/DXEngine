#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"
#include "UObject.h"
#include "UScene.h"
#include "UCamera.h"

IMPLEMENT_UCLASS(UGizmoManager, UEngineSubsystem)
UGizmoManager::UGizmoManager()
{
}

UGizmoManager::~UGizmoManager()
{
	gridPrimitive = nullptr;

	for (auto gizmo : locationGizmos)
	{
		delete gizmo;
	}
	locationGizmos.clear();
	for (auto gizmo : rotationGizmos)
	{
		delete gizmo;
	}
	rotationGizmos.clear();
	for (auto gizmo : scaleGizmos)
	{
		delete gizmo;
	}
	scaleGizmos.clear();
}

bool UGizmoManager::Initialize(UMeshManager* meshManager)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = new UGizmoGridComp();

	// =================================================

	UGizmoArrowComp* arrowX = new UGizmoArrowComp();
	arrowX->Axis = EAxis::X;
	UGizmoArrowComp* arrowY = new UGizmoArrowComp();
	arrowY->Axis = EAxis::Y;
	UGizmoArrowComp* arrowZ = new UGizmoArrowComp();
	arrowZ->Axis = EAxis::Z;

	arrowX->SetColor({ 1, 0, 0, 1 });
	arrowY->SetColor({ 0, 1, 0, 1 });
	arrowZ->SetColor({ 0, 0, 1, 1 });

	arrowX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	arrowY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	arrowZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	locationGizmos.push_back(arrowX);
	locationGizmos.push_back(arrowZ);
	locationGizmos.push_back(arrowY);

	// =================================================

	UGizmoRotationHandleComp* rotationX = new UGizmoRotationHandleComp();
	rotationX->Axis = EAxis::X;
	UGizmoRotationHandleComp* rotationY = new UGizmoRotationHandleComp();
	rotationY->Axis = EAxis::Y;
	UGizmoRotationHandleComp* rotationZ = new UGizmoRotationHandleComp();
	rotationZ->Axis = EAxis::Z;

	rotationX->SetColor({ 1, 0, 0, 1 });
	rotationY->SetColor({ 0, 1, 0, 1 });
	rotationZ->SetColor({ 0, 0, 1, 1 });

	rotationX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	rotationY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	rotationZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	rotationGizmos.push_back(rotationX);
	rotationGizmos.push_back(rotationZ);
	rotationGizmos.push_back(rotationY);

	// =================================================

	UGizmoScaleHandleComp* scaleX = new UGizmoScaleHandleComp();
	scaleX->Axis = EAxis::X;
	UGizmoScaleHandleComp* scaleY = new UGizmoScaleHandleComp();
	scaleY->Axis = EAxis::Y;
	UGizmoScaleHandleComp* scaleZ = new UGizmoScaleHandleComp();
	scaleZ->Axis = EAxis::Z;

	scaleX->SetColor({ 1, 0, 0, 1 });
	scaleY->SetColor({ 0, 1, 0, 1 });
	scaleZ->SetColor({ 0, 0, 1, 1 });

	scaleX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	scaleY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	scaleZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	scaleGizmos.push_back(scaleX);
	scaleGizmos.push_back(scaleZ);
	scaleGizmos.push_back(scaleY);

	// =================================================

	if (!gridPrimitive->Init() || !arrowX->Init() || !arrowY->Init() || !arrowZ->Init()
		|| !rotationX->Init() || !rotationY->Init() || !rotationZ->Init()
		|| !scaleX->Init() || !scaleY->Init() || !scaleZ->Init())
	{
		delete gridPrimitive;

		delete arrowX;
		delete arrowY;
		delete arrowZ;

		delete rotationX;
		delete rotationY;
		delete rotationZ;

		delete scaleX;
		delete scaleY;
		delete scaleZ;

		return false;
	}

	return true;
}

void UGizmoManager::SetTarget(UPrimitiveComponent* target)
{
	targetObject = target;
}

void UGizmoManager::ChangeGizmoSpace()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Space");
		return;
	}

	isWorldSpace = !isWorldSpace;
}

void UGizmoManager::NextTranslation()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Translation");
		return;
	}

	switch (translationType)
	{
	case ETranslationType::Location:
		translationType = ETranslationType::Rotation;
		break;
	case ETranslationType::Rotation:
		translationType = ETranslationType::Scale;
		break;
	case ETranslationType::Scale:
		translationType = ETranslationType::Location;
		break;
	}
}

TArray<UGizmoComponent*>& UGizmoManager::GetRaycastableGizmos()
{
	if (targetObject == nullptr)
	{
		static TArray<UGizmoComponent*> emptyArray; // lives for the whole program
		return emptyArray;
	}

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<UGizmoComponent*>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}
	return *currentGizmos;
}

void UGizmoManager::Update(float deltaTime)
{
	if (gridPrimitive)
	{
		gridPrimitive->Update(deltaTime);
	}
}

void UGizmoManager::Draw(URenderer& renderer)
{
	// --- 파트 1: 타겟 유무와 상관없이 항상 그리는 요소 ---
	if (gridPrimitive)
	{
		gridPrimitive->Draw(renderer);
	}

	// --- 파트 2: 타겟이 있을 때만 그리는 요소 ---

	if (targetObject == nullptr) return;

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<UGizmoComponent*>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}

	if (currentGizmos)
	{
		for (auto gizmoPart : *currentGizmos)
		{
			if (gizmoPart)
			{
				// Todo: 이동을 update에서 처리해야 하나??
				gizmoPart->SetPosition(targetObject->GetPosition());

				// Scale 은 항상 local
				if (translationType != ETranslationType::Scale && isWorldSpace)
				{
					gizmoPart->ResetQuaternion();
				}
				else
				{
					gizmoPart->SetQuaternion(targetObject->RelativeQuaternion);
				}

				float gizmoScale = (targetObject->RelativeLocation - camera->GetLocation()).Length() * 0.15f;
				gizmoPart->SetScale({ gizmoScale, gizmoScale, gizmoScale });

				gizmoPart->DrawOnTop(renderer);
			}
		}
	}
}
// FVector2, FVector4, FMatrix4x4는 사용하시는 엔진의 자료형으로 대체하세요.


FVector ConvertWorldVectorToCamera2D(const FVector& worldVector, const UCamera* camera)
{
	// 1. 카메라의 로컬 축 벡터를 가져옵니다.
	FVector cameraRight = camera->GetRight();
	FVector cameraUp = camera->GetUp();

	// 2. 월드 벡터를 카메라의 Right축과 Up축에 각각 내적합니다.
	float newX = worldVector.Dot(cameraRight);
	float newY = worldVector.Dot(cameraUp);

	// 3. 계산된 값으로 2D 벡터를 반환합니다.
	return FVector(newX, newY, 0);
}

FVector WorldVectorToScreenVector(
	const FVector& worldVector,
	const FMatrix& viewMatrix,
	const FMatrix& projectionMatrix,
	float screenWidth,
	float screenHeight)
{
	// 1. 월드 벡터를 4차원 벡터로 변환 (동차 좌표계, w=1은 위치, w=0은 방향)
	// 방향 벡터를 변환할 때는 w=0을 사용해야 이동(Translation)이 적용되지 않습니다.
	FVector4 worldPos_4D(worldVector.X, worldVector.Y, worldVector.Z, 0.0f);

	// 2. 뷰 변환과 투영 변환을 순차적으로 적용
	// DirectX 기준 순서입니다. OpenGL은 순서가 반대일 수 있습니다.
	FMatrix viewProjectionMatrix = viewMatrix * projectionMatrix;
	FVector4 clipPos = FMatrix::MultiplyVectorRow(worldPos_4D, viewProjectionMatrix);

	// 3. 원근 나누기 (w로 나누어 정규화된 장치 좌표(NDC)로 변환)
	// w가 0에 가까우면 (카메라와 매우 가깝거나 뒤에 있으면) 연산을 피합니다.
	if (abs(clipPos.W) < 0.0001f)
	{
		return FVector(-1.0f, -1.0f, 0); // 화면 밖이라는 의미의 값
	}
	FVector ndcPos(clipPos.X / clipPos.W, clipPos.Y / clipPos.W, clipPos.Z / clipPos.W);

	// 4. 뷰포트 변환 (NDC -> Screen Coordinates)
	float screenX = (ndcPos.X + 1.0f) / 2.0f * screenWidth;
	float screenY = (1.0f - ndcPos.Y) / 2.0f * screenHeight;

	return FVector(screenX, screenY, 0);
}

//레이와 이동 평면의 3D 교차점 찾기
FVector UGizmoManager::FindCirclePlaneIntersection(const FRay& ray, const FPlane& plane)
{
	float denominator = ray.Direction.Dot(plane.Normal);

	// 레이가 평면과 평행에 가까우면 계산 오류 방지
	if (abs(denominator) < 0.0001f) return { 0,0,0 };

	float t = (plane.PointOnPlane - ray.Origin).Dot(plane.Normal) / denominator;
	FVector intersectionPoint = ray.Origin + ray.Direction * t;

	return intersectionPoint;
}

void UGizmoManager::BeginDrag(const FRay& mouseRay, EAxis axis, FVector impactPoint, UScene* curScene)
{
	isDragging = true;
	selectedAxis = axis;

	dragStartLocation = targetObject->GetPosition();
	dragStartScale = targetObject->GetScale();
	dragStartQuaternion = targetObject->GetQuaternion();

	// 이동 평면 생성
	movementPlane.PointOnPlane = dragStartLocation;

	// 사용자가 선택한 이동 축 벡터 (예: X축이면 (1,0,0))
	FVector axisDir = GetAxisVector(selectedAxis);

	// 카메라 위치에서 객체를 향하는 벡터 (시선 벡터)
	FVector camToObjectDir = (dragStartLocation - mouseRay.Origin).Normalized();

	if (translationType == ETranslationType::Rotation)
	{
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		}
		dragRotationStartPoint = mouseRay.MousePos;
		FVector rotDir = axisDir.Cross(impactPoint - dragStartLocation);
		dragRotationStartVector = ConvertWorldVectorToCamera2D(rotDir, curScene->GetCamera()).Normalized();
		return;
	}
	else if (translationType == ETranslationType::Location)
	{
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		}

		// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
		// 다시 외적하여 평면의 법선 벡터를 계산
		FVector tempVec = axisDir.Cross(camToObjectDir);
		movementPlane.Normal = axisDir.Cross(tempVec).Normalized();
	}
	else if (translationType == ETranslationType::Scale)
	{
		// Scale 은 항상 로컬 스페이스 모드
		axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);

		// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
		// 다시 외적하여 평면의 법선 벡터를 계산
		FVector tempVec = axisDir.Cross(camToObjectDir);
		movementPlane.Normal = axisDir.Cross(tempVec).Normalized();
	}

	// 선택한 곳을 offset 으로 저장해서 드래그 시 중심점으로 이동하지 않게 하기
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	float projectedLength = startToIntersectionVec.Dot(axisDir);

	projectedLengthOffset = projectedLength;

}

void UGizmoManager::UpdateDrag(const FRay& mouseRay)
{
	if (!isDragging) return;

	// --- 1. 마우스 레이와 이동 평면의 3D 교차점 찾기 ---
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	FVector axisDir = GetAxisVector(selectedAxis);

	if (translationType == ETranslationType::Location)
	{
		// --- [위치] 업데이트 로직 ---
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		}

		float projectedLength = startToIntersectionVec.Dot(axisDir) - projectedLengthOffset;
		FVector newPosition = dragStartLocation + axisDir * projectedLength;

		targetObject->SetPosition(newPosition);
	}
	else if (translationType == ETranslationType::Scale)
	{
		axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		float projectedLength = startToIntersectionVec.Dot(axisDir) - projectedLengthOffset;
		FVector newScale = dragStartScale + GetAxisVector(selectedAxis) * projectedLength;

		const float minimumScale = 0.1f;

		newScale.X = max(newScale.X, minimumScale);
		newScale.Y = max(newScale.Y, minimumScale);
		newScale.Z = max(newScale.Z, minimumScale);

		targetObject->SetScale(newScale);
	}
	else // ETranslationType::Rotation
	{
		FVector mouseOffset = mouseRay.MousePos - dragRotationStartPoint;

		mouseOffset.X *= -1;

		float newAmout = mouseOffset.Dot(dragRotationStartVector);

		const float rotationSensitivity = 0.005f;
		float angle = newAmout * rotationSensitivity;

		// 계산된 각도를 적용하여 최종 회전값 계산
		FQuaternion finalQuaternion;
		FVector rotationAxis = GetAxisVector(selectedAxis);

		if (isWorldSpace)
		{
			finalQuaternion = dragStartQuaternion.RotatedWorldAxisAngle(rotationAxis, angle);
		}
		else
		{
			finalQuaternion = dragStartQuaternion.RotatedLocalAxisAngle(rotationAxis, angle);
		}

		targetObject->SetQuaternion(finalQuaternion);
	}
}

void UGizmoManager::EndDrag()
{
	isDragging = false;
	selectedAxis = EAxis::None;
	if (gridPrimitive)
	{
		gridPrimitive->bIsSelected = false;
	}
}

void UGizmoManager::SetTranslationType(ETranslationType type)
{
	if (isDragging)
	{
		return;
	}

	translationType = type;
}

void UGizmoManager::SetGizmoSpace(bool isWorld)
{
	if (isDragging)
	{
		return;
	}

	isWorldSpace = isWorld;
}

FVector UGizmoManager::GetAxisVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 1,0,0 };
	case EAxis::Y:
		return { 0,1,0 };
	case EAxis::Z:
		return { 0,0,1 };
	default:
		return { 0,0,0 };
	}
}

FVector UGizmoManager::GetRotationVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 0,0,1 };
	case EAxis::Y:
		return { 1,0,0 };
	case EAxis::Z:
		return { 0,1,0 };
	default:
		return { 0,0,0 };
	}
}