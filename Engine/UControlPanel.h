#pragma once
#include "ImGuiWindowWrapper.h"

class USceneComponent;
class USceneManager;
class UGizmoManager;
class URenderer;
class UControlPanel : public ImGuiWindowWrapper
{
	USceneManager* SceneManager;
	UGizmoManager* GizmoManager;
	URenderer* Renderer;

	// Spawn Primitive Section
	TArray<UClass*> registeredTypes;
	TArray<FString> choiceStrList;
	TArray<const char*> choices;
	int32 primitiveChoiceIndex = 0;

	// Scene Management Section
	char sceneName[256] = "Default";

	// Camera Management Section
	bool isSolid = true;
public:
	UControlPanel(USceneManager* sceneManager, UGizmoManager* gizmoManager, URenderer* renderer);
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
	void GridManagementSection();

	USceneComponent* CreateSceneComponentFromChoice(int index);

	// ================================================================= //
	// Newly added feature
	void PerformanceSection();
};

