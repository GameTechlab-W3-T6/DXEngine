#include "stdafx.h"
#include "AActor.h"
#include "UClass.h"
#include "UObjectMacros.h"
#include "UActorComponent.h"
#include "USceneComponent.h"
#include "UEngineStatics.h"

IMPLEMENT_UCLASS(AActor, UObject)

AActor::AActor()
    : RootComponent(nullptr)
{
    UUID = UEngineStatics::GenUUID();
    ID = ActorID++;
    name = "Actor_" + std::to_string(ID);
}


void AActor::SetRootComponent(USceneComponent* InRootComponent)
{
    RootComponent = InRootComponent;
}

bool AActor::Initialize()
{
    for (auto& comp : Components)
    {
        if (comp && !comp->IsInitialized())
        {
            comp->Internal_Initialize();
        }
    }

    return true;
}

void AActor::Update(float deltaTime)
{
    for (auto& comp : Components)
    {
        if (comp && comp->IsActive() && comp->IsTickEnabled())
        {
            comp->Internal_Update(deltaTime);
        }
    }
}

void AActor::OnShutdown()
{
    for (auto& comp : Components)
    {
        if (comp && !comp->IsShutdown())
        {
            comp->Internal_Shutdown();
        }
    }
}

json::JSON AActor::Serialize() const
{
    json::JSON result = UObject::Serialize();

    result["Type"] = "AActor";
    result["ActorClass"] = GetClass()->GetDisplayName();

    // Serialize components
    int32 componentCount = 0;
    for (const auto& comp : Components)
    {
        if (comp)
        {
            json::JSON compJson = comp->Serialize();
            if (!compJson.IsNull())
            {
                result["Components"][std::to_string(componentCount)] = compJson;
                ++componentCount;
            }
        }
    }

    return result;
}

bool AActor::Deserialize(const json::JSON& data)
{
    if (!UObject::Deserialize(data))
        return false;

    // Deserialize components if they exist
    if (data.hasKey("Components"))
    {
        json::JSON componentsJson = data.at("Components");
        for (auto& compPair : componentsJson.ObjectRange())
        {
            json::JSON compData = compPair.second;

            if (compData.hasKey("Type"))
            {
                FString compType = compData.at("Type").ToString();
                UClass* compClass = UClass::FindClassWithDisplayName(compType);

                if (compClass)
                {
                    if (UActorComponent* comp = compClass->CreateDefaultObject()->Cast<UActorComponent>())
                    {
                        comp->Deserialize(compData);
                        // Note: Components are managed by unique_ptr, this is simplified
                        // In a full implementation, you'd need proper component management
                    }
                }
            }
        }
    }

    return true;
}
