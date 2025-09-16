#include "stdafx.h"
#include "UClass.h"
#include "USphereComp.h"
#include "UPrimitiveComponent.h"

IMPLEMENT_UCLASS(USphereComp, UPrimitiveComponent)
UCLASS_META(USphereComp, DisplayName, "Sphere")
UCLASS_META(USphereComp, MeshName, "Sphere")

uint32 USphereComp::SphereID = 0;
