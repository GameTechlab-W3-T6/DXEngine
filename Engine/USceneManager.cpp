#include "stdafx.h"
#include "USceneManager.h"
#include "UScene.h"
#include "UApplication.h"


IMPLEMENT_UCLASS(USceneManager, UEngineSubsystem)
USceneManager::~USceneManager()
{
	if (currentScene)
		delete currentScene;
}

bool USceneManager::Initialize(UApplication* _application)
{
	application = _application;
	currentScene = _application->CreateDefaultScene();
	currentScene->Initialize(
		&application->GetRenderer(),
		&application->GetMeshManager(),
		&application->GetInputManager());
	return true;
}

UScene* USceneManager::GetScene()
{
	return currentScene;
}

void USceneManager::SetScene(UScene* scene)
{
	if (currentScene != nullptr)
	{
		delete currentScene;
	}

	currentScene = scene;

	currentScene->Initialize(
		&application->GetRenderer(),
		&application->GetMeshManager(),
		&application->GetInputManager());

	application->OnSceneChange();
}

void USceneManager::RequestExit()
{
	if (application) {
		application->RequestExit();
	}
}


void USceneManager::LoadScene(const FString& path)
{
	std::ifstream file(path);
	if (!file)
	{
		// Log error: failed to open file
		return;

	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	json::JSON sceneData = json::JSON::Load(buffer.str());
	SetScene(UScene::Create(sceneData));
}

void USceneManager::SaveScene(const FString& path)
{
	std::filesystem::path fsPath(path);

	if (std::filesystem::exists(fsPath))
	{
		std::ifstream file(path);
		if (file)
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			json::JSON sceneData = json::JSON::Load(buffer.str());

			currentScene->SetVersion(sceneData["Version"].ToInt() + 1);
		}
	}

	json::JSON sceneData = currentScene->Serialize();

	std::ofstream file(path);

	if (!file)
	{
		// Log error: failed to open file
		return;
	}

	file << sceneData.dump();
}

