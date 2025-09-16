#include "stdafx.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"
#include "AStaticMeshActor.h"

void UDefaultScene::Update(float deltaTime)
{
    UScene::Update(deltaTime);
    static float t = 0.0f;
    t += deltaTime;
}

bool UDefaultScene::OnInitialize()
{
    UScene::OnInitialize();
    if (IsFirstTime)
    {
        // Create default sphere actor
        AStaticMeshActor* sphereActor = AStaticMeshActor::CreateSphere();
        if (sphereActor)
        {
            USceneComponent* rootComponent = sphereActor->GetRootComponent();
            if (rootComponent)
            {
                rootComponent->SetPosition({ 0.0f, 0.0f, 0.0f });
                rootComponent->SetRotation({ 0.0f, 0.0f, 0.0f });
                rootComponent->SetScale({ 0.5f, 0.5f, 0.5f });
            }
            AddActor(sphereActor);
        }
        IsFirstTime = false;
    }

	return true;
}
