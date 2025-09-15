#include "stdafx.h"
#include "UClass.h"
#include "UGizmoComponent.h"
#include "UGizmoRotationHandleComp.h"

IMPLEMENT_UCLASS(UGizmoRotationHandleComp, UGizmoComponent)
UCLASS_META(UGizmoRotationHandleComp, MeshName, "GizmoRotationHandle")
UCLASS_META(UGizmoRotationHandleComp, VertexShaderName, "GizmoVertex")
UCLASS_META(UGizmoRotationHandleComp, PixelShaderName, "GizmoPixel")

UGizmoRotationHandleComp::UGizmoRotationHandleComp()
{
}