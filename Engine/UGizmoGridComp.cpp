#include "stdafx.h"
#include "UClass.h"
#include "UGizmoComponent.h"
#include "UGizmoGridComp.h"

IMPLEMENT_UCLASS(UGizmoGridComp, UGizmoComponent)
UCLASS_META(UGizmoGridComp, MeshName, "GizmoGrid")

UGizmoGridComp::UGizmoGridComp()
{
}

void UGizmoGridComp::Update(float deltaTime)
{
	// TODO : fix those temporary set size feature
	ConfigData* config = ConfigManager::GetConfig("editor");
	float gridSize = config->getFloat("Gizmo", "GridSize");

	SetScale({ gridSize, gridSize, gridSize });
}
