#pragma once

/** @todo: Substitute headers with forward declarations. */
#include "ImGuiWindowWrapper.h"
#include "UScene.h"
#include "USceneManager.h"
#include "USceneComponent.h"

class EditorApplication;

/**
  * @note: Every Scene Component in current scene appears on the window.
  *		   But, it is possible to select only PrimitiveComponent.
  */
class USceneManagerWindow : public ImGuiWindowWrapper
{
public:
	USceneManagerWindow(EditorApplication* Editor)
		: ImGuiWindowWrapper("Scene Manager", ImVec2(270, 0), ImVec2(300, 450)), Editor(Editor)
	{
		assert(Editor);
	}

protected:
	/** @todo: Is it necessary to disable gizmo on Scene Manager Window? */
	virtual void RenderContent() override;

private:
	EditorApplication* Editor;
};
