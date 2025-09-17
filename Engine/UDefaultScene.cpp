#include "stdafx.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"

void UDefaultScene::Update(float deltaTime)
{
    UScene::Update(deltaTime);
    static float t = 0.0f;
    t += deltaTime;
}

bool UDefaultScene::OnInitialize()
{
    UScene::OnInitialize();
    //if (IsFirstTime)
    //{
    //    // 컴포넌트 생성
    //    USphereComp* sphere = new USphereComp({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });

    //    AddObject(sphere);
    //    IsFirstTime = false;
    //}

	return true;
}
