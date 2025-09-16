// USceneComponent.cpp
#include "stdafx.h"
#include "USceneComponent.h"
#include "json.hpp"
#include "UClass.h"
#include "UInputManager.h"
#include "UScene.h"

IMPLEMENT_UCLASS(USceneComponent, UObject)
FMatrix USceneComponent::GetWorldTransform()
{
    return FMatrix::SRTRowQuaternion(RelativeLocation, RelativeQuaternion.ToMatrixRow(), RelativeScale3D);
}

bool USceneComponent::Initialize()
{
    UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
     inputManager->RegisterKeyCallback(std::to_string(InternalIndex), [this](int32 keyCode) {HandleInput(keyCode);});
    
    return true;
}

void USceneComponent::Update(float deltaTime)
{

}

void USceneComponent::OnShutdown()
{
    UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
    inputManager->UnregisterCallbacks(std::to_string(InternalIndex));
}

void USceneComponent::HandleInput(int32 keyCode)
{
    if (keyCode == VK_DELETE && bIsSelected)
    {
        markedAsDestroyed = true;
    }
}

json::JSON USceneComponent::Serialize() const
{
    FVector tmpRot = RelativeQuaternion.GetEulerXYZ();
    json::JSON result;
    result["Location"] = json::Array(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
    result["Rotation"] = json::Array( tmpRot.X, tmpRot.Y, tmpRot.Z);
    result["Scale"] = json::Array(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);
    result["Type"] = GetClass()->GetDisplayName();
    return result;
}

// -------------------------
// Deserialize
// -------------------------
bool USceneComponent::Deserialize(const json::JSON& data)
{
    if (!data.hasKey("Location") || !data.hasKey("Rotation") || !data.hasKey("Scale"))
        return false;

    auto loc = data.at("Location");
    if (loc.size() != 3) return false;
    RelativeLocation = FVector(loc[0].ToFloat(), loc[1].ToFloat(), loc[2].ToFloat());

    auto rot = data.at("Rotation");
    if (rot.size() != 3) return false;
    RelativeQuaternion = FQuaternion::FromEulerXYZ(rot[0].ToFloat(), rot[1].ToFloat(), rot[2].ToFloat());

    auto scale = data.at("Scale");
    if (scale.size() != 3) return false;
    RelativeScale3D = FVector(scale[0].ToFloat(), scale[1].ToFloat(), scale[2].ToFloat());

    return true;
}