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

	// Handle input in organized sections
	HandleKeyboardInput();
	HandleCameraInput(deltaTime);
	HandleMouseInput();
}

void EditorApplication::HandleKeyboardInput()
{
	UInputManager& inputManager = GetInputManager();

	// Application controls
	if (inputManager.IsKeyDown(VK_ESCAPE))
	{
		RequestExit();
	}

	// Gizmo controls
	if (inputManager.IsKeyPressed(VK_SPACE))
	{
		gizmoManager.NextTranslation();
	}

	if (inputManager.IsKeyPressed('X'))
	{
		gizmoManager.ChangeGizmoSpace();
	}
}

void EditorApplication::HandleCameraInput(float deltaTime)
{
	UInputManager& inputManager = GetInputManager();
	UScene* scene = GetSceneManager().GetScene();
	if (!scene) return;

	UCamera* camera = scene->GetCamera();
	if (!camera) return;

	const float baseSensitivity = config->getFloat("Camera", "Sensitivity") * deltaTime / 100.0f;

	// Mouse look handling
	if (inputManager.IsMouseLooking())
	{
		float mdx = 0.f, mdy = 0.f;
		inputManager.ConsumeMouseDelta(mdx, mdy);
		const float mouseSensitivity = baseSensitivity;
		camera->AddYawPitch(mdx * mouseSensitivity, mdy * mouseSensitivity);
	}

	// Movement input
	FVector movementInput(0.f, 0.f, 0.f);
	
	if (inputManager.IsKeyDown('W')) movementInput.X += 1.0f;  // Forward
	if (inputManager.IsKeyDown('S')) movementInput.X -= 1.0f;  // Backward
	if (inputManager.IsKeyDown('A')) movementInput.Y -= 1.0f;  // Left
	if (inputManager.IsKeyDown('D')) movementInput.Y += 1.0f;  // Right
	if (inputManager.IsKeyDown('E')) movementInput.Z += 1.0f;  // Up
	if (inputManager.IsKeyDown('Q')) movementInput.Z -= 1.0f;  // Down

	// Normalize diagonal movement
	float inputLength = sqrtf(movementInput.X * movementInput.X + 
							 movementInput.Y * movementInput.Y + 
							 movementInput.Z * movementInput.Z);
	
	if (inputLength > 0.f)
	{
		movementInput.X /= inputLength;
		movementInput.Y /= inputLength;
		movementInput.Z /= inputLength;

		bool boost = inputManager.IsKeyDown(VK_SHIFT);
		camera->MoveLocal(movementInput.X, movementInput.Y, movementInput.Z, deltaTime, boost);
	}
}

void EditorApplication::HandleMouseInput()
{
	UInputManager& inputManager = GetInputManager();

	// Early exit if ImGui wants mouse control
	if (ImGui::GetIO().WantCaptureMouse) return;

	// Handle mouse release - end drag operations
	if (inputManager.IsMouseButtonReleased(0))
	{
		EndDragOperation();
		return;
	}

	// Handle ongoing drag operations
	if (gizmoManager.IsDragging())
	{
		UpdateDragOperation();
		return;
	}

	// Handle mouse press - start new selection/drag
	if (inputManager.IsMouseButtonPressed(0))
	{
		HandleMouseClick();
	}
}

void EditorApplication::EndDragOperation()
{
	// Clear gizmo selections
	TArray<UGizmoComponent*>& gizmos = gizmoManager.GetRaycastableGizmos();
	for (UGizmoComponent* gizmo : gizmos)
	{
		gizmo->bIsSelected = false;
	}
	gizmoManager.EndDrag();
}

void EditorApplication::UpdateDragOperation()
{
	UCamera* camera = GetSceneManager().GetScene()->GetCamera();
	if (!camera) return;

	FRay ray = GetRaycastManager().CreateRayFromScreenPosition(camera);
	gizmoManager.UpdateDrag(ray);
}

void EditorApplication::HandleMouseClick()
{
	TArray<UPrimitiveComponent*> primitives;
	TArray<UGizmoComponent*> gizmos;

	// Collect raycastable objects
	CollectRaycastableObjects(gizmos, primitives);

	// Check for gizmo hits first (higher priority)
	FVector impactPoint;
	UGizmoComponent* hitGizmo = nullptr;
	UPrimitiveComponent* hitPrimitive = nullptr;

	if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), gizmos, hitGizmo, impactPoint))
	{
		HandleGizmoHit(hitGizmo, impactPoint);
	}
	else if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), primitives, hitPrimitive, impactPoint))
	{
		HandlePrimitiveHit(hitPrimitive);
	}
	else
	{
		HandleEmptySpaceClick();
	}
}

void EditorApplication::CollectRaycastableObjects(TArray<UGizmoComponent*>& outGizmos, TArray<UPrimitiveComponent*>& outPrimitives)
{
	// Collect gizmos
	TArray<UGizmoComponent*>& gizmos = gizmoManager.GetRaycastableGizmos();
	for (UGizmoComponent* gizmo : gizmos)
	{
		outGizmos.push_back(gizmo);
		gizmo->bIsSelected = false;
	}

	// Collect primitives
	for (UObject* obj : GetSceneManager().GetScene()->GetObjects())
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			if (primitive->GetMesh())
			{
				outPrimitives.push_back(primitive);
			}
			primitive->bIsSelected = false;
		}
	}
}

void EditorApplication::HandleGizmoHit(UGizmoComponent* hitGizmo, const FVector& impactPoint)
{
	auto target = gizmoManager.GetTarget();
	if (!target) return;

	target->bIsSelected = true;
	hitGizmo->bIsSelected = true;

	FRay ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());

	// Handle different gizmo types
	if (UGizmoArrowComp* arrow = hitGizmo->Cast<UGizmoArrowComp>())
	{
		gizmoManager.BeginDrag(ray, arrow->Axis, impactPoint, GetSceneManager().GetScene());
	}
	else if (UGizmoRotationHandleComp* rotationHandle = hitGizmo->Cast<UGizmoRotationHandleComp>())
	{
		gizmoManager.BeginDrag(ray, rotationHandle->Axis, impactPoint, GetSceneManager().GetScene());
	}
	else if (UGizmoScaleHandleComp* scaleHandle = hitGizmo->Cast<UGizmoScaleHandleComp>())
	{
		gizmoManager.BeginDrag(ray, scaleHandle->Axis, impactPoint, GetSceneManager().GetScene());
	}

	if (target->IsManageable())
	{
		propertyWindow->SetTarget(target);
	}
}

void EditorApplication::HandlePrimitiveHit(UPrimitiveComponent* hitPrimitive)
{
	HandlePrimitiveSelect(hitPrimitive);
}

void EditorApplication::HandleEmptySpaceClick()
{
	gizmoManager.SetTarget(nullptr);
	propertyWindow->SetTarget(nullptr);

}

void EditorApplication::Render()
{
	UApplication::Render();
	gizmoManager.Draw(GetRenderer());
}

void EditorApplication::RenderGUI()
{
	controlPanel->Render();
	propertyWindow->Render();
	SceneManagerWindow->Render();

	ImGui::SetNextWindowPos(ImVec2(0, 560));         // Fixed position (x=20, y=20)
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

	controlPanel = new UControlPanel(&GetSceneManager(), &gizmoManager, &GetRenderer());
	propertyWindow = new USceneComponentPropertyWindow();
	SceneManagerWindow = MakeUnique<USceneManagerWindow>(this);
	config = ConfigManager::GetConfig("editor");

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

void EditorApplication::HandlePrimitiveSelect(UPrimitiveComponent* Component)
{
	if (selectedSceneComponent)
	{
		selectedSceneComponent->bIsSelected = false;
	}
	selectedSceneComponent = Component;
	gizmoManager.SetTarget(Component);
	Component->bIsSelected = true;

	if (Component->IsManageable())
	{
		propertyWindow->SetTarget(Component);
	}
}
