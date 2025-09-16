#pragma once
#include "UObject.h"
#include "UEngineStatics.h"

class AActor;

enum class EComponentLifecycleState : uint8
{
    Created,
    Initialized,
    Active,
    Shutdown,
    Destroyed
};

/**
 * @brief Base class for all actor components
 */
class UActorComponent : public UObject
{
    DECLARE_UCLASS(UActorComponent, UObject)

public:
    UActorComponent();
    virtual ~UActorComponent() = default;

    // Current lifecycle methods (keep existing names)
    virtual bool Initialize() { return true; }  // TODO: Rename to InitializeComponent() later
    virtual void Update(float deltaTime) {}     // TODO: Rename to TickComponent() later
    virtual void OnShutdown() {}                // TODO: Rename to EndPlay() later

    // Component state management
    bool IsInitialized() const { return LifecycleState >= EComponentLifecycleState::Initialized; }
    bool IsActive() const { return LifecycleState == EComponentLifecycleState::Active; }
    bool IsShutdown() const { return LifecycleState >= EComponentLifecycleState::Shutdown; }

    EComponentLifecycleState GetLifecycleState() const { return LifecycleState; }

    // Owner management
    AActor* GetOwner() const { return Owner; }
    void SetOwner(AActor* InOwner) { Owner = InOwner; }

    // Component control
    void SetTickEnabled(bool bEnabled) { bTickEnabled = bEnabled; }  // TODO: Rename to SetComponentTickEnabled() later
    bool IsTickEnabled() const { return bTickEnabled; }             // TODO: Rename to IsComponentTickEnabled() later

protected:
    AActor* Owner = nullptr;
    EComponentLifecycleState LifecycleState = EComponentLifecycleState::Created;
    bool bTickEnabled = true;

private:
    friend class AActor;

    // Internal lifecycle management
    void Internal_Initialize();
    void Internal_Update(float deltaTime);
    void Internal_Shutdown();
};

