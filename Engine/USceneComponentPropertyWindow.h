#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"
#include "USceneComponent.h"

class USceneComponentPropertyWindow : public ImGuiWindowWrapper
{
private:
	USceneComponent* Target = nullptr;

public:
	USceneComponentPropertyWindow() : ImGuiWindowWrapper("Property Window", ImVec2(0, 450), ImVec2(275, 110))
	{

	}
	void SetTarget(USceneComponent* target) 
	{
		Target = target;
	}
	void RenderContent() override;
};

