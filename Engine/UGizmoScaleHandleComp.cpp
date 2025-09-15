#include "stdafx.h"
#include "UClass.h"
#include "UGizmoComponent.h"
#include "UGizmoScaleHandleComp.h"

IMPLEMENT_UCLASS(UGizmoScaleHandleComp, UGizmoComponent)
UCLASS_META(UGizmoScaleHandleComp, MeshName, "GizmoScaleHandle")
UCLASS_META(UGizmoScaleHandleComp, VertexShaderName, "GizmoVertex")
UCLASS_META(UGizmoScaleHandleComp, PixelShaderName, "GizmoPixel")

UGizmoScaleHandleComp::UGizmoScaleHandleComp()
{
}