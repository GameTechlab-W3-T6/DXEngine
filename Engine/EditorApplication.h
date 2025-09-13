#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphereComp.h"
#include "URaycastManager.h"
#include "UControlPanel.h"
#include "USceneComponentPropertyWindow.h"

// Simple application that inherits from UApplication
class EditorApplication : public UApplication
{
private:
	UGizmoManager gizmoManager;
	TArray<USceneComponent*> sceneComponents;

	USceneComponent* selectedSceneComponent;

	UControlPanel* controlPanel;
	USceneComponentPropertyWindow* propertyWindow;

public:
	EditorApplication() = default;
	~EditorApplication()
	{
		delete controlPanel;
		delete propertyWindow;
		controlPanel = nullptr;
		propertyWindow = nullptr;
	}
	UScene* CreateDefaultScene() override;

	void OnSceneChange() override;

protected:
	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int32 width, int32 height) override;

private:
	// Input handling methods
	void HandleKeyboardInput();
	void HandleCameraInput(float deltaTime);
	void HandleMouseInput();

	// Mouse interaction methods
	void EndDragOperation();
	void UpdateDragOperation();
	void HandleMouseClick();
	void CollectRaycastableObjects(TArray<UGizmoComponent*>& outGizmos, TArray<UPrimitiveComponent*>& outPrimitives);
	void HandleGizmoHit(UGizmoComponent* hitGizmo, const FVector& impactPoint);
	void HandlePrimitiveHit(UPrimitiveComponent* hitPrimitive);
	void HandleEmptySpaceClick();
	  
	bool AABBFlag = false;
	FVector minPos, maxPos;

};