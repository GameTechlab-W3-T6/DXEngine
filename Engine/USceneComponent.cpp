// USceneComponent.cpp
#include "stdafx.h"
#include "USceneComponent.h"
#include "json.hpp"
#include "UClass.h"
#include "UInputManager.h"
#include "UScene.h"
#include "AActor.h"

IMPLEMENT_UCLASS(USceneComponent, UActorComponent)
FMatrix USceneComponent::GetWorldTransform() const
{
    FMatrix relativeTransform = GetRelativeTransform();

    if (AttachParent)
    {
        return relativeTransform * AttachParent->GetWorldTransform();
    }

    return relativeTransform;
}

FMatrix USceneComponent::GetRelativeTransform() const
{
    return FMatrix::SRTRowQuaternion(RelativeLocation, RelativeQuaternion.ToMatrixRow(), RelativeScale3D);
}

void USceneComponent::AttachToComponent(USceneComponent* Parent)
{
    if (!Parent || Parent == this)
        return;

    // Detach from current parent first
    DetachFromComponent();

    // Set new parent
    AttachParent = Parent;
    Parent->AttachChildren.push_back(this);
}

void USceneComponent::AttachChild(USceneComponent* Child)
{
    if (!Child || Child == this)
        return;

    // Make the child attach to this component
    Child->AttachToComponent(this);
}

void USceneComponent::DetachFromComponent()
{
    if (AttachParent)
    {
        // Remove from parent's children list
        auto& children = AttachParent->AttachChildren;
        for (int i = 0; i < children.size(); ++i)
        {
            if (children[i] == this)
            {
                children.erase(children.begin() + i);
                break;
            }
        }
        AttachParent = nullptr;
    }
}

void USceneComponent::DetachAllChildren()
{
    // Detach all children (iterate backwards to avoid index issues)
    for (int i = AttachChildren.size() - 1; i >= 0; --i)
    {
        if (AttachChildren[i])
        {
            AttachChildren[i]->DetachFromComponent();
        }
    }
    AttachChildren.clear();
}

bool USceneComponent::IsAttachedTo(USceneComponent* Component) const
{
    if (!Component)
        return false;

    USceneComponent* current = AttachParent;
    while (current)
    {
        if (current == Component)
            return true;
        current = current->AttachParent;
    }
    return false;
}

USceneComponent* USceneComponent::GetRootComponent() const
{
    USceneComponent* root = const_cast<USceneComponent*>(this);
    while (root->AttachParent)
    {
        root = root->AttachParent;
    }
    return root;
}

void USceneComponent::GetAllChildren(TArray<USceneComponent*>& OutChildren, bool bIncludeNestedChildren) const
{
    for (auto* child : AttachChildren)
    {
        if (child)
        {
            OutChildren.push_back(child);
            if (bIncludeNestedChildren)
            {
                child->GetAllChildren(OutChildren, true);
            }
        }
    }
}

FVector USceneComponent::GetWorldLocation() const
{
    FMatrix worldTransform = GetWorldTransform();
    // Extract translation from matrix (4th column in row-major format)
    return FVector(worldTransform.M[3][0], worldTransform.M[3][1], worldTransform.M[3][2]);
}

FQuaternion USceneComponent::GetWorldRotation() const
{
    if (AttachParent)
    {
        // In LH Z-up, quaternion multiplication order is important
        FQuaternion parentWorldRotation = AttachParent->GetWorldRotation();
        return parentWorldRotation * RelativeQuaternion;
    }
    return RelativeQuaternion;
}

FVector USceneComponent::GetWorldScale() const
{
    if (AttachParent)
    {
        FVector parentWorldScale = AttachParent->GetWorldScale();
        // Component-wise multiplication for scale
        return FVector(
            RelativeScale3D.X * parentWorldScale.X,
            RelativeScale3D.Y * parentWorldScale.Y,
            RelativeScale3D.Z * parentWorldScale.Z
        );
    }
    return RelativeScale3D;
}

void USceneComponent::SetWorldLocation(const FVector& NewLocation)
{
    if (AttachParent)
    {
        // Convert world location to relative location using parent's inverse transform
        FMatrix parentWorldTransform = AttachParent->GetWorldTransform();
        FMatrix invParentTransform = FMatrix::Inverse(parentWorldTransform);
        // Use row-based transformation (LH Z-up compatible)
        FVector relativeLocation = invParentTransform.TransformPointRow(NewLocation);
        SetPosition(relativeLocation);
    }
    else
    {
        SetPosition(NewLocation);
    }
}

void USceneComponent::SetWorldRotation(const FQuaternion& NewRotation)
{
    if (AttachParent)
    {
        // Convert world rotation to relative rotation
        FQuaternion parentWorldRotation = AttachParent->GetWorldRotation();
        // In LH Z-up: Relative = Parent^-1 * World
        FQuaternion relativeRotation = parentWorldRotation.Inverse() * NewRotation;
        SetQuaternion(relativeRotation);
    }
    else
    {
        SetQuaternion(NewRotation);
    }
}

void USceneComponent::SetWorldScale(const FVector& NewScale)
{
    if (AttachParent)
    {
        // Convert world scale to relative scale
        FVector parentWorldScale = AttachParent->GetWorldScale();
        // Component-wise division, avoiding divide by zero
        const float SMALL_NUMBER = 0.0001f;
        FVector relativeScale = FVector(
            abs(parentWorldScale.X) > SMALL_NUMBER ? NewScale.X / parentWorldScale.X : NewScale.X,
            abs(parentWorldScale.Y) > SMALL_NUMBER ? NewScale.Y / parentWorldScale.Y : NewScale.Y,
            abs(parentWorldScale.Z) > SMALL_NUMBER ? NewScale.Z / parentWorldScale.Z : NewScale.Z
        );
        SetScale(relativeScale);
    }
    else
    {
        SetScale(NewScale);
    }
}

bool USceneComponent::Initialize()
{
    // Call parent initialization first
    if (!Super::Initialize())
        return false;

    UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
    inputManager->RegisterKeyCallback(std::to_string(InternalIndex), [this](int32 keyCode) {HandleInput(keyCode);});

    return true;
}

void USceneComponent::Update(float deltaTime)
{
    // Call parent update first
    Super::Update(deltaTime);

    // Update all attached child components
    for (USceneComponent* child : AttachChildren)
    {
        if (child)
        {
            child->Update(deltaTime);
        }
    }

    // SceneComponent-specific update logic can go here
}

void USceneComponent::OnShutdown()
{
    // Shutdown and cleanup all attached child components
    for (USceneComponent* child : AttachChildren)
    {
        if (child)
        {
            child->OnShutdown();
            delete child;
        }
    }
    AttachChildren.clear();

    UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
    inputManager->UnregisterCallbacks(std::to_string(InternalIndex));

    // Detach from parent during shutdown
    DetachFromComponent();
    // Call parent shutdown
    Super::OnShutdown();
}

void USceneComponent::HandleInput(int32 keyCode)
{
    if (keyCode == VK_DELETE && bIsSelected)
    {
        // If this component belongs to an Actor, mark the Actor for destruction
        if (AActor* owner = GetOwner())
        {
            owner->markedAsDestroyed = true;
        }
        else
        {
            // Legacy component not owned by Actor
            markedAsDestroyed = true;
        }
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