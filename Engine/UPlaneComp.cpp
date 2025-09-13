#include "stdafx.h"
#include "UClass.h"
#include "UPrimitiveComponent.h"
#include "UPlaneComp.h"
#include "UObject.h"
#include "UInputManager.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

UPlaneComp::UPlaneComp(FVector pos, FVector rot, FVector scl)
	:UPrimitiveComponent(pos, rot, scl)
{

	UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
	if (inputManager)
	{
		inputManager->RegisterKeyCallback(std::to_string(InternalIndex), [](int32 keyData) {
			if (keyData == VK_SPACE)
				UE_LOG("test");
			}
		);
	}

}

UPlaneComp::~UPlaneComp()

{
	UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();

	if (inputManager)
	{
		inputManager->UnregisterCallbacks(std::to_string(InternalIndex));
	}
}
UCLASS_META(UPlaneComp, TextureType, "PlaneBaseColor")
UCLASS_META(UPlaneComp, TextInfo, "TextInfo");