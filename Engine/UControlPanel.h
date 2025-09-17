#pragma once
#include "ImGuiWindowWrapper.h"
#include "ConfigData.h"
#include "Constant.h"

class USceneComponent;
class AActor;
class AStaticMeshActor;
class USceneManager;
class UGizmoManager;
class URenderer;
class UControlPanel : public ImGuiWindowWrapper
{
	USceneManager* SceneManager;
	UGizmoManager* GizmoManager;
	URenderer* Renderer;
	
	ConfigData* config;

	// Spawn Primitive Section
	TArray<UClass*> registeredTypes;  // TODO: Deprecated, remove after migration
	TArray<FString> choiceStrList;
	TArray<const char*> choices;
	int32 primitiveChoiceIndex = 0;

	// New mesh-based primitive section
	TArray<FString> availableMeshNames;
	TArray<FString> displayNames;
	TArray<const char*> meshChoices;
	int32 meshChoiceIndex = 0;

	// Scene Management Section
	char sceneName[256] = "Default";

	// Camera Management Section
	EViewModeIndex CurrentViewMode = EViewModeIndex::VMI_Lit;
    bool showPrims = true, showText = true;

public:
	UControlPanel(USceneManager* sceneManager, UGizmoManager* gizmoManager, URenderer* renderer);
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
	void ViewManagementSection();
	void GridManagementSection();

	USceneComponent* CreateSceneComponentFromChoice(int index);  // TODO: Deprecated, use CreateActorFromChoice
	AActor* CreateActorFromChoice(int index);

	// New mesh-based creation
	void InitializeMeshBasedPrimitives();
	AActor* CreateActorFromMeshChoice(int index);
	FString GetDisplayNameForMesh(const FString& meshName);

	// ================================================================= //
	// Newly added feature
	void PerformanceSection();
};

