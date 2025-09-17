#pragma once
#include "UScene.h"
#include "USphereComp.h"
#include "UGizmoGridComp.h"
class UDefaultScene :
    public UScene
{
public:
    void Update(float deltaTime) override;
    bool OnInitialize() override;
};

 