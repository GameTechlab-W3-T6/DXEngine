#include "stdafx.h"
#include "UApplication.h"
#include "EditorApplication.h"
#include "UMeshManager.h"
#include "ImguiConsole.h"
#include "UScene.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"

void EditorApplication::Update(float deltaTime)
{
	// Basic update logic
	UApplication::Update(deltaTime);
	gizmoManager.Update(deltaTime);

	if (GetInputManager().IsKeyDown(VK_ESCAPE))
	{
		RequestExit();
	}

	if (GetInputManager().IsKeyPressed(VK_SPACE))
	{
		gizmoManager.NextTranslation();
	}

	if (GetInputManager().IsKeyPressed('X'))
	{
		gizmoManager.ChangeGizmoSpace();
	}

	FVector outImpactPoint;
	UGizmoComponent* hitGizmo = nullptr;
	UPrimitiveComponent* hitPrimitive = nullptr;

	if (GetInputManager().IsMouseButtonReleased(0))
	{
		TArray<UGizmoComponent*> gizmos;

		TArray<UGizmoComponent*>& g = gizmoManager.GetRaycastableGizmos();
		if (g.size() > 0)
		{
			for (UGizmoComponent* gizmo : g)
			{
				gizmos.push_back(gizmo);
				gizmo->bIsSelected = false;
			}
		}
		gizmoManager.EndDrag();
		return;
	}

	// 드래그 하고 있을때
	if (gizmoManager.IsDragging())
	{
		FRay ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());
		gizmoManager.UpdateDrag(ray);
		return;
	}

	if (ImGui::GetIO().WantCaptureMouse) return;

	if (GetInputManager().IsMouseButtonPressed(0))
	{
		TArray<UPrimitiveComponent*> primitives;
		TArray<UGizmoComponent*> gizmos;

		TArray<UGizmoComponent*>& g = gizmoManager.GetRaycastableGizmos();
		if (g.size() > 0)
		{
			for (UGizmoComponent* gizmo : g)
			{
				gizmos.push_back(gizmo);
				gizmo->bIsSelected = false;
			}
		}

		for (UObject* obj : GetSceneManager().GetScene()->GetObjects())
		{
			if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
			{
				if (primitive->GetMesh()) primitives.push_back(primitive);
				primitive->bIsSelected = false;
			}
		}

		// std::cout << "gizmos.size() : " << gizmos.size();
		// std::cout << " primitives.size() : " << primitives.size() << std::endl;
		if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), gizmos, hitGizmo, outImpactPoint))
		{
			// std::cout << "hitGizmo : " << hitGizmo << std::endl;
			if (auto target = gizmoManager.GetTarget())
			{
				target->bIsSelected = true;
				hitGizmo->bIsSelected = true;
				UGizmoArrowComp* arrow = hitGizmo->Cast<UGizmoArrowComp>();

				FRay ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());

				// UGizmoArrowComp로 캐스팅 시도
				if (UGizmoArrowComp* arrow = hitGizmo->Cast<UGizmoArrowComp>())
				{
					gizmoManager.BeginDrag(ray, arrow->Axis, outImpactPoint, GetSceneManager().GetScene());
				}
				// UGizmoRotationHandleComp로 캐스팅 시도
				else if (UGizmoRotationHandleComp* rotationHandle = hitGizmo->Cast<UGizmoRotationHandleComp>())
				{
					gizmoManager.BeginDrag(ray, rotationHandle->Axis, outImpactPoint, GetSceneManager().GetScene()); // 스케일 드래그 시작 로직 추가
				}
				// UGizmoScaleHandleComp로 캐스팅 시도
				else if (UGizmoScaleHandleComp* scaleHandle = hitGizmo->Cast<UGizmoScaleHandleComp>())
				{
					gizmoManager.BeginDrag(ray, scaleHandle->Axis, outImpactPoint, GetSceneManager().GetScene()); // 스케일 드래그 시작 로직 추가
				}

				if (target->IsManageable())
					propertyWindow->SetTarget(target);
			}
		}
		else if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), primitives, hitPrimitive, outImpactPoint))
		{
			gizmoManager.SetTarget(hitPrimitive);
			hitPrimitive->bIsSelected = true;
			if (hitPrimitive->IsManageable())
				propertyWindow->SetTarget(hitPrimitive);
		}
		else
		{
			gizmoManager.SetTarget(nullptr);
			propertyWindow->SetTarget(nullptr);
		}
	}
}

void EditorApplication::Render(bool bIsShaderReflectionEnabled)
{
	UApplication::Render(bIsShaderReflectionEnabled);
	gizmoManager.Draw(GetRenderer(), bIsShaderReflectionEnabled);
}

void EditorApplication::RenderGUI()
{
	controlPanel->Render();
	propertyWindow->Render();

	ImGui::SetNextWindowPos(ImVec2(0, 500));         // Fixed position (x=20, y=20)
	ImGui::SetNextWindowSize(ImVec2(275, 75));      // Fixed size (width=300, height=100)
	ImGui::Begin("Memory Stats", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);                // Prevent resizing, moving, collapsing

	ImGui::Text("Allocated Object Count : %d", UEngineStatics::GetTotalAllocationCount());
	ImGui::Text("Allocated Object Bytes : %d", UEngineStatics::GetTotalAllocationBytes());

	ImGui::End();

	bool isConsoleOpen = false;
	// static ImguiConsole imguiConsole;
	GConsole.Draw("Console", &isConsoleOpen);
}

bool EditorApplication::OnInitialize()
{
	UApplication::OnInitialize();
	// 리사이즈/초기화

	controlPanel = new UControlPanel(&GetSceneManager(), &gizmoManager);
	propertyWindow = new USceneComponentPropertyWindow();

	if (!gizmoManager.Initialize(&GetMeshManager()))
	{
		MessageBox(GetWindowHandle(), L"Failed to initialize gizmo manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	gizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());

	return true;
}


void EditorApplication::OnResize(int32 width, int32 height)
{
	UScene* scene = GetSceneManager().GetScene();
	if (scene == nullptr) return;

	UCamera* camera = scene->GetCamera();
	if (camera == nullptr) return;

	camera->SetPerspectiveDegrees(
		camera->GetFOV(),
		(height > 0) ? (float)width / (float)height : 1.0f,
		camera->GetNearZ(),
		camera->GetFarZ());
}

UScene* EditorApplication::CreateDefaultScene()
{
	return new UDefaultScene();
}

void EditorApplication::OnSceneChange()
{
	propertyWindow->SetTarget(nullptr);
	gizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());
	gizmoManager.SetTarget(nullptr);
}
