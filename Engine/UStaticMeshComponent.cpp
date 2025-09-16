#include "stdafx.h"
#include "UStaticMeshComponent.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UStaticMeshComponent, UPrimitiveComponent)

uint32 UStaticMeshComponent::StaticMeshID = 0;

UStaticMeshComponent::UStaticMeshComponent(FVector pos, FVector rot, FVector scl)
    : UPrimitiveComponent(pos, rot, scl)
{
    Name = GetDefaultName();
    ID = StaticMeshID++;
}

void UStaticMeshComponent::SetMeshType(const FString& meshName, const FString& displayName)
{
    // Update metadata dynamically
    GetClass()->SetMeta("MeshName", meshName);
    if (!displayName.empty())
    {
        GetClass()->SetMeta("DisplayName", displayName);
    }
}

void UStaticMeshComponent::SetupAsCube()
{
    SetMeshType("Cube", "Cube");
    Name = "Cube";
}

void UStaticMeshComponent::SetupAsSphere()
{
    SetMeshType("Sphere", "Sphere");
    Name = "Sphere";
}

void UStaticMeshComponent::SetupAsPlane()
{
    SetMeshType("Plane", "Plane");
    Name = "Plane";
}