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
    return UScene::OnInitialize();
}
