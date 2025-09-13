#include "stdafx.h"
#include "UControlPanel.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "USceneManager.h"
#include "UScene.h"
#include "UDefaultScene.h"

// 활성화(선택) 상태면 버튼색을 Active 계열로 바꿔서 '눌린 버튼'처럼 보이게 하는 헬퍼
static bool ModeButton(const char* label, bool active, const ImVec2& size = ImVec2(0, 0))
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 colBtn = active ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_Button];
	ImVec4 colHover = active ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_ButtonHovered];
	ImVec4 colActive = style.Colors[ImGuiCol_ButtonActive];

	ImGui::PushStyleColor(ImGuiCol_Button, colBtn);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colActive);
	bool pressed = ImGui::Button(label, size);
	ImGui::PopStyleColor(3);
	return pressed;
}

UControlPanel::UControlPanel(USceneManager* sceneManager, UGizmoManager* gizmoManager)
	: ImGuiWindowWrapper("Control Panel", ImVec2(0, 0), ImVec2(275, 390)), SceneManager(sceneManager), GizmoManager(gizmoManager)
{
	for (const auto& registeredType : UClass::GetClassList())
	{
		if (!registeredType->IsChildOrSelfOf(USceneComponent::StaticClass()))
			continue;

		FString displayName = registeredType->GetMeta("DisplayName");
		if (displayName.empty())
			continue;

		registeredTypes.push_back(registeredType.get());
		choiceStrList.push_back(registeredType->GetMeta("DisplayName"));
	}

	for (const FString& str : choiceStrList)
	{
		choices.push_back(str.c_str());
	}
}

void UControlPanel::RenderContent()
{
	PrimaryInformationSection();
	ImGui::Separator();
	SpawnPrimitiveSection();
	ImGui::Separator();
	SceneManagementSection();
	ImGui::Separator();
	CameraManagementSection();
}

void UControlPanel::PrimaryInformationSection()
{
	float frameRate = ImGui::GetIO().Framerate;
	ImGui::Text("FPS %.0f (%.0f ms)", frameRate, 1000.0f / frameRate);
}

USceneComponent* UControlPanel::CreateSceneComponentFromChoice(int index)
{
	auto obj = registeredTypes[primitiveChoiceIndex]->CreateDefaultObject();
	if (!obj) return nullptr;
	return obj->Cast<USceneComponent>();
}

void UControlPanel::SpawnPrimitiveSection()
{
	ImGui::SetNextItemWidth(150);
	ImGui::Combo("Type", &primitiveChoiceIndex, choices.data(), static_cast<int32>(choices.size()));

	int32 objectCount = SceneManager->GetScene()->GetObjectCount();
	if (ImGui::Button("Spawn"))
	{
		USceneComponent* sceneComponent = CreateSceneComponentFromChoice(primitiveChoiceIndex);
		if (sceneComponent != nullptr)
		{
			sceneComponent->SetPosition(FVector(
				-5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f,
				-5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f,
				-5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f
			));
			sceneComponent->SetScale(FVector(
				0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f,
				0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f,
				0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f
			));
			sceneComponent->SetRotation(FVector(
				-90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f,
				-90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f,
				-90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f
			));
			AActor* actor = new AActor();
			// TODO : Implement AddComponentByUClass
			//actor->AddComponentByClass<USceneComponent>();
			SceneManager->GetScene()->AddObject(actor);
		}
	}
	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::SetNextItemWidth(60);
	ImGui::SameLine();
	ImGui::InputInt("Spawned", &objectCount, 0);
	ImGui::EndDisabled();
}

void UControlPanel::SceneManagementSection()
{
	ImGui::Text("Scene Name");                // Label on top
	ImGui::SetNextItemWidth(200);             // Optional: set input width
	ImGui::InputText("##SceneNameInput", sceneName, sizeof(sceneName)); // invisible label

	if (ImGui::Button("New scene"))
	{
		// TODO : Make New Scene
		SceneManager->SetScene(new UDefaultScene());

	}
	ImGui::SameLine();
	if (ImGui::Button("Save scene") && strcmp(sceneName, "") != 0)
	{
		std::filesystem::path _path("./data/");
		std::filesystem::create_directory(_path);
		SceneManager->SaveScene(_path.string() + FString(sceneName) + ".Scene");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load scene") && strcmp(sceneName, "") != 0)
	{
		SceneManager->LoadScene("./data/" + FString(sceneName) + ".Scene");
	}
}

void UControlPanel::CameraManagementSection()
{
	UCamera* camera = SceneManager->GetScene()->GetCamera();
	// 카메라 정보
	FVector pos = camera->GetLocation();
	float cameraLocation[3] = { pos.X, pos.Y, pos.Z };
	FVector eulDeg = camera->GetEulerXYZDeg();
	float eulerXYZ[3] = { eulDeg.X, eulDeg.Y, eulDeg.Z };

	// --- 테이블 UI ---
	bool locCommitted = false;
	bool rotCommitted = false;

	bool isOrthogonal = camera->IsOrtho();
	ImGui::Checkbox("Orthogonal", &isOrthogonal);
	if (isOrthogonal)
	{
		// 원하는 직교 크기로 (예시: 월드 단위 10x10)
		camera->SetOrtho(10.0f, 10.0f, camera->GetNearZ(), camera->GetFarZ(), /*leftHanded=*/true);
	}
	else
	{
		camera->SetPerspectiveDegrees(camera->GetFOV(),
			camera->GetAspect(), camera->GetNearZ(), camera->GetFarZ());
	}

	// === FOV (perspective일 때만 활성화) ===
	ImGui::TextUnformatted("FOV");

	float fovDeg = camera->GetFOV();
	bool changed = false;

	const float minFov = 10.0f;
	const float maxFov = 120.0f;
	const float dragSpeed = 0.2f; // 픽셀당 증가량(°)

	float avail = ImGui::GetContentRegionAvail().x;

	ImGui::BeginDisabled(isOrthogonal);
	ImGui::PushID("FOV");

	// 드래그 박스 (좌우 드래그로 값 변경)
	ImGui::SetNextItemWidth(avail * 0.55f);
	changed |= ImGui::DragFloat("##fov_drag", &fovDeg, dragSpeed, minFov, maxFov, "%.1f",
		ImGuiSliderFlags_AlwaysClamp);

	// 리셋 버튼(옵션)
	ImGui::SameLine();
	if (ImGui::Button("Reset")) { fovDeg = 60.0f; changed = true; }

	if (changed) {
		fovDeg = std::clamp(fovDeg, minFov, maxFov);
		camera->SetFOV(fovDeg); // 내부에서 proj 재빌드
	}

	ImGui::PopID();
	ImGui::EndDisabled();



	// --- Euler(XYZ) 편집 ---
	// Location
	ImGui::Text("Camera Location");
	if (ImGui::BeginTable("EditableCameraTable", 3, ImGuiTableFlags_None))
	{
		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);

			// DragFloat로 교체
			if (ImGui::DragFloat(("##loc" + std::to_string(i)).c_str(),
				&cameraLocation[i], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
			{
				locCommitted = true; // 값이 바뀐 순간 바로 commit
			}
			// 만약 "편집 종료 후만" commit 원하면 IsItemDeactivatedAfterEdit() 체크로 바꾸기
		}
		ImGui::EndTable();
	}

	//ImGui::Text("Camera Rotation");
	//if (ImGui::BeginTable("CameraRotTable", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
	//{
	//	ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
	//	ImGui::TableSetColumnIndex(0); ImGui::Text("Pitch");
	//	ImGui::TableSetColumnIndex(1); ImGui::Text("Yaw");
	//	ImGui::TableSetColumnIndex(2); ImGui::Text("Roll");

	//	// Camera Rotation 행
	//	ImGui::TableNextRow();
	//	for (int32 i = 0; i < 3; i++)
	//	{
	//		ImGui::TableSetColumnIndex(i);
	//		ImGui::SetNextItemWidth(-1);
	//		ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
	//			&eulerXYZ[i], 0.0f, 0.0f, "%.3f");
	//		if (ImGui::IsItemDeactivatedAfterEdit())
	//			rotCommitted = true;
	//	}
	//	// ImGui::TableSetColumnIndex(3);

	//	ImGui::EndTable();
	//}

	// World / Local 선택 (체크박스 대신, 버튼처럼 보이는 상호배타 토글)

	if (ModeButton("World", GizmoManager->GetIsWorldSpace()))
	{
		GizmoManager->SetGizmoSpace(true);
	}
	ImGui::SameLine();
	if (ModeButton("Local", !GizmoManager->GetIsWorldSpace()))
	{
		GizmoManager->SetGizmoSpace(false);
	}
	/* 모드/좌표계 토글 렌더링 */
	if (ModeButton("Translation", GizmoManager->GetTranslationType() == ETranslationType::Location))
	{
		GizmoManager->SetTranslationType(ETranslationType::Location);
	}
	ImGui::SameLine();
	if (ModeButton("Rotation", GizmoManager->GetTranslationType() == ETranslationType::Rotation))
	{
		GizmoManager->SetTranslationType(ETranslationType::Rotation);
	}
	ImGui::SameLine();
	if (ModeButton("Scale", GizmoManager->GetTranslationType() == ETranslationType::Scale))
	{
		GizmoManager->SetTranslationType(ETranslationType::Scale);
	}

	// === 변경사항을 카메라에 반영 ===
	// 위치
	// === 변경사항을 카메라에 '커밋 시'만 반영 ===
	if (locCommitted)
	{
		camera->SetLocation(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
	}

	if (rotCommitted)
	{
		camera->SetEulerXYZDeg(eulerXYZ[0], eulerXYZ[1], eulerXYZ[2]);
	}
}
