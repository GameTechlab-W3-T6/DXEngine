#include "stdafx.h"
#include "UClass.h"
#include "UPrimitiveComponent.h"
#include "UPlaneComp.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")
UCLASS_META(UPlaneComp, TextureType, "PlaneBaseColor")
UCLASS_META(UPlaneComp, TextInfo, "TextInfo");