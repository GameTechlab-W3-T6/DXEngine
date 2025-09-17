#include "stdafx.h"
#include "UActorComponent.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UActorComponent, UObject)

UActorComponent::UActorComponent()
    : Owner(nullptr), LifecycleState(EComponentLifecycleState::Created), bTickEnabled(true)
{
}

void UActorComponent::Internal_Initialize()
{
    if (LifecycleState != EComponentLifecycleState::Created)
        return;

    if (Initialize())
    {
        LifecycleState = EComponentLifecycleState::Initialized;
        // TODO: Add BeginPlay equivalent later
        LifecycleState = EComponentLifecycleState::Active;
    }
}

void UActorComponent::Internal_Update(float deltaTime)
{
    if (LifecycleState == EComponentLifecycleState::Active && bTickEnabled)
    {
        Update(deltaTime);
    }
}

void UActorComponent::Internal_Shutdown()
{
    if (LifecycleState >= EComponentLifecycleState::Shutdown)
        return;

    LifecycleState = EComponentLifecycleState::Shutdown;
    OnShutdown();
    LifecycleState = EComponentLifecycleState::Destroyed;
}
