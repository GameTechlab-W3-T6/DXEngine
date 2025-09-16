#include "stdafx.h"

#include "EditorApplication.h"
#include "USceneManagerWindow.h"

void USceneManagerWindow::RenderContent()
{
	UScene* Scene = Editor->GetSceneManager().GetScene();
	for (const auto& Object : Scene->GetObjects())
	{
		auto Component = Object->Cast<UPrimitiveComponent>();
		/** @note: If Object is not PrimitiveComponent, then it is ignored. */
		if (!Component || !Component->IsManageable())
		{
			continue;
		}
		FString Name = Component->GetName() + std::to_string(Component->GetID());
		if (ImGui::Selectable(Name.c_str(), Component->bIsSelected))
		{
			Editor->HandlePrimitiveSelect(Component);
		}
	}
}
