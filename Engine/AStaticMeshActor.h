#pragma once
#include "AActor.h"
#include "UStaticMeshComponent.h"

/**
 * @brief Actor class that contains a static mesh component
 * Replaces the direct component creation in scenes
 */
class AStaticMeshActor : public AActor
{
    DECLARE_UCLASS(AStaticMeshActor, AActor)

public:
    AStaticMeshActor();
    virtual ~AStaticMeshActor() = default;

    // Lifecycle methods (using existing names)
    virtual bool Initialize() override;  // TODO: Rename to InitializeComponent() later

    // Static mesh management
    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

    // Helper functions to create specific mesh types
    static AStaticMeshActor* CreateCube(FVector pos = {0, 0, 0}, FVector rot = {0, 0, 0}, FVector scl = {1, 1, 1});
    static AStaticMeshActor* CreateSphere(FVector pos = {0, 0, 0}, FVector rot = {0, 0, 0}, FVector scl = {1, 1, 1});
    static AStaticMeshActor* CreatePlane(FVector pos = {0, 0, 0}, FVector rot = {0, 0, 0}, FVector scl = {1, 1, 1});

    // Set mesh type for this actor
    void SetMeshType(const FString& meshName, const FString& displayName = "");
    void SetTransform(FVector pos, FVector rot = {0, 0, 0}, FVector scl = {1, 1, 1});

protected:
    UStaticMeshComponent* StaticMeshComponent = nullptr;
};