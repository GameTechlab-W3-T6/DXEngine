#include "stdafx.h"
#include "UClass.h"
#include "UGizmoArrowComp.h"
#include "UGizmoGridComp.h"

IMPLEMENT_UCLASS(UGizmoArrowComp, UGizmoComponent)
UCLASS_META(UGizmoArrowComp, MeshName, "GizmoArrow")
UCLASS_META(UGizmoArrowComp, VertexShaderName, "GizmoVertex")
UCLASS_META(UGizmoArrowComp, PixelShaderName, "GizmoPixel")

UGizmoArrowComp::UGizmoArrowComp()
{

}