#include "stdafx.h"
#include "AStaticMeshActor.h"
#include "UClass.h"

IMPLEMENT_UCLASS(AStaticMeshActor, AActor)

AStaticMeshActor::AStaticMeshActor()
{
    // Create and add static mesh component
    auto meshComp = MakeUnique<UStaticMeshComponent>();
    StaticMeshComponent = AddComponent(std::move(meshComp));
}

bool AStaticMeshActor::Initialize()
{
    // Call parent initialization first
    if (!Super::Initialize())
        return false;

    return true;
}

void AStaticMeshActor::SetMeshType(const FString& meshName, const FString& displayName)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetMeshType(meshName, displayName);
    }
}

void AStaticMeshActor::SetTransform(FVector pos, FVector rot, FVector scl)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetPosition(pos);
        StaticMeshComponent->SetRotation(rot);
        StaticMeshComponent->SetScale(scl);
    }
}

AStaticMeshActor* AStaticMeshActor::CreateCube(FVector pos, FVector rot, FVector scl)
{
    AStaticMeshActor* actor = new AStaticMeshActor();
    actor->SetTransform(pos, rot, scl);
    actor->SetMeshType("Cube", "Cube");
    if (actor->GetStaticMeshComponent())
    {
        actor->GetStaticMeshComponent()->SetupAsCube();
    }
    return actor;
}

AStaticMeshActor* AStaticMeshActor::CreateSphere(FVector pos, FVector rot, FVector scl)
{
    AStaticMeshActor* actor = new AStaticMeshActor();
    actor->SetTransform(pos, rot, scl);
    actor->SetMeshType("Sphere", "Sphere");
    if (actor->GetStaticMeshComponent())
    {
        actor->GetStaticMeshComponent()->SetupAsSphere();
    }
    return actor;
}

AStaticMeshActor* AStaticMeshActor::CreatePlane(FVector pos, FVector rot, FVector scl)
{
    AStaticMeshActor* actor = new AStaticMeshActor();
    actor->SetTransform(pos, rot, scl);
    actor->SetMeshType("Plane", "Plane");
    if (actor->GetStaticMeshComponent())
    {
        actor->GetStaticMeshComponent()->SetupAsPlane();
    }
    return actor;
}