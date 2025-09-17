#pragma once
#include "UPrimitiveComponent.h"

/**
 * @brief Generic static mesh component that can render different mesh types
 * Uses existing MeshManager, TextureManager, ShaderManager system via metadata
 */
class UStaticMeshComponent : public UPrimitiveComponent
{
    DECLARE_UCLASS(UStaticMeshComponent, UPrimitiveComponent)

public:
    UStaticMeshComponent(FVector pos = {0, 0, 0}, FVector rot = {0, 0, 0}, FVector scl = {1, 1, 1});
    virtual ~UStaticMeshComponent() = default;

    // Set mesh type after construction (updates metadata)
    void SetMeshType(const FString& meshName, const FString& displayName = "");

    // Helper functions for common mesh types
    void SetupAsCube();
    void SetupAsSphere();
    void SetupAsPlane();

    virtual uint32 GetID() const { return ID; }
    virtual bool IsManageable() override { return true; }

protected:
    virtual const char* GetDefaultName() const override { return "StaticMesh"; }

private:
    static uint32 StaticMeshID;
    uint32 ID;
};