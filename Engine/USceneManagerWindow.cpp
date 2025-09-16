#include "stdafx.h"

#include "EditorApplication.h"
#include "USceneManagerWindow.h"
#include "AActor.h"

void USceneManagerWindow::RenderContent()
{
	UScene* Scene = Editor->GetSceneManager().GetScene();

	// Display Actors first
	for (AActor* actor : Scene->GetActors())
	{
		if (!actor) continue;

		FString actorName = "Actor_" + std::to_string(actor->UUID);

		// Create tree node for actor
		if (ImGui::TreeNode(actorName.c_str()))
		{
			// Display actor's components as children
			auto allComponents = actor->GetComponents<UActorComponent>();
			for (UActorComponent* comp : allComponents)
			{
				if (UPrimitiveComponent* primitive = comp->Cast<UPrimitiveComponent>())
				{
					FString componentName = "  " + primitive->GetName() + "_" + std::to_string(primitive->GetID());
					bool isSelectable = primitive->IsManageable();

					if (isSelectable)
					{
						if (ImGui::Selectable(componentName.c_str(), primitive->bIsSelected))
						{
							Editor->HandlePrimitiveSelect(primitive);
						}
					}
					else
					{
						// Show non-manageable components (like UTextholderComp) but make them non-selectable
						ImGui::TextDisabled("%s", componentName.c_str());
					}
				}
			}
			ImGui::TreePop();
		}
	}

	// Display legacy objects (components not owned by actors)
	bool hasLegacyObjects = false;
	for (const auto& Object : Scene->GetObjects())
	{
		auto Component = Object->Cast<UPrimitiveComponent>();
		if (!Component || !Component->IsManageable())
		{
			continue;
		}

		// Only show if not owned by an actor
		if (!Component->GetOwner())
		{
			if (!hasLegacyObjects)
			{
				ImGui::Separator();
				ImGui::Text("Legacy Objects:");
				hasLegacyObjects = true;
			}

			FString Name = Component->GetName() + std::to_string(Component->GetID());
			if (ImGui::Selectable(Name.c_str(), Component->bIsSelected))
			{
				Editor->HandlePrimitiveSelect(Component);
			}
		}
	}
}
