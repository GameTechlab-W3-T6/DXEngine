// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "AActor.h"
#include "UApplication.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UCamera.h"
#include "Constant.h"

IMPLEMENT_UCLASS(UScene, UObject)
UScene::UScene()
{
	version = 1;
	primitiveCount = 0;
}

UScene::~UScene()
{
	OnShutdown();
	for (UObject* object : objects)
	{
		delete object;
	}
	delete camera;
}

bool UScene::Initialize(UApplication* app, URenderer* r, UMeshManager* mm, UInputManager* im)
{
	application = app;
	renderer = r;
	meshManager = mm;
	inputManager = im;

	backBufferWidth = 0.0f;
	backBufferHeight = 0.0f;

	// 모든 Primitive 컴포넌트 초기화
	for (UObject* obj : objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			bool success = primitive->Initialize();
			if (!success) return false;
		}
	}

	camera = new UCamera();
	camera->SetPerspectiveDegrees(60.0f, (backBufferHeight > 0) ? (float)backBufferWidth / (float)backBufferHeight : 1.0f, 0.1f, 1000.0f);
	camera->LookAt({ -5,0,0 }, { 0,0,0 }, { 0,0,1 });

	return OnInitialize();
}

UScene* UScene::Create(json::JSON data)
{
	UScene* scene = new UScene();
	scene->Deserialize(data);
	return scene;
}

void UScene::AddObject(USceneComponent* obj)
{
	// 런타임에서만 사용 - Scene이 Initialize된 후에 호출할 것
	assert(meshManager != nullptr && "AddObject should only be called after Scene initialization");

	if (!meshManager)
	{
		// 릴리즈 빌드에서 안전성 확보
		return;
	}

	objects.push_back(obj);

	// 일단 표준 RTTI 사용
	if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
	{
		primitive->Initialize();
		if (obj->CountOnInspector())
			++primitiveCount;
	}
}

void UScene::RemoveObject(USceneComponent* obj)
{
	auto itr = std::find(objects.begin(), objects.end(), obj);

	if (itr == objects.end()) return;

	objects.erase(itr);
	--primitiveCount;
}

json::JSON UScene::Serialize() const
{
	json::JSON result;
	// UScene 특성에 맞는 JSON 구성
	result["Version"] = version;
	result["NextUUID"] = std::to_string(UEngineStatics::GetNextUUID());

	// Serialize legacy objects (components)
	int32 validCount = 0;
	for (UObject* object : objects)
	{
		if (object == nullptr) continue;
		json::JSON _json = object->Serialize();
		if (!_json.IsNull())
		{
			result["Primitives"][std::to_string(object->UUID)] = _json;
			++validCount;
		}
	}

	// Serialize actors
	int32 validActorCount = 0;
	for (AActor* actor : actors)
	{
		if (actor == nullptr) continue;
		json::JSON actorJson = actor->Serialize();
		if (!actorJson.IsNull())
		{
			result["Actors"][std::to_string(actor->UUID)] = actorJson;
			++validActorCount;
		}
	}

	return result;
}

bool UScene::Deserialize(const json::JSON& data)
{
	version = data.at("Version").ToInt();
	//nextUUID = data.at("NextUUID").ToInt();

	objects.clear();
	actors.clear();

	if (!data.hasKey("Primitives")) return false;
	json::JSON primitivesJson = data.at("Primitives");

	UEngineStatics::SetUUIDGeneration(false);
	UObject::ClearFreeIndices();
	for (auto& primitiveJson : primitivesJson.ObjectRange())
	{
		uint32 uuid = stoi(primitiveJson.first);
		json::JSON _data = primitiveJson.second;

		UClass* _class = UClass::FindClassWithDisplayName(_data.at("Type").ToString());
		USceneComponent* component = nullptr;
			if(_class != nullptr) component = _class->CreateDefaultObject()->Cast<USceneComponent>();

		component->Deserialize(_data);
		component->SetUUID(uuid);

		objects.push_back(component);
		if (component->CountOnInspector())
			++primitiveCount;
	}

	// Deserialize actors if they exist
	if (data.hasKey("Actors"))
	{
		json::JSON actorsJson = data.at("Actors");
		for (auto& actorJson : actorsJson.ObjectRange())
		{
			uint32 uuid = stoi(actorJson.first);
			json::JSON actorData = actorJson.second;

			if (actorData.hasKey("ActorClass"))
			{
				FString actorClassName = actorData.at("ActorClass").ToString();
				UClass* actorClass = UClass::FindClassWithDisplayName(actorClassName);

				if (actorClass)
				{
					if (AActor* actor = actorClass->CreateDefaultObject()->Cast<AActor>())
					{
						actor->Deserialize(actorData);
						actor->SetUUID(uuid);
						actors.push_back(actor);
					}
				}
			}
		}
	}

	USceneComponent* gizmoGrid = new UGizmoGridComp(
		{ 0.3f, 0.3f, 0.3f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.2f, 0.2f, 0.2f }
	);
	//objects.push_back(gizmoGrid);

	FString uuidStr = data.at("NextUUID").ToString();

	UEngineStatics::SetNextUUID((uint32)stoi(uuidStr));
	UEngineStatics::SetUUIDGeneration(true);

	return true;
}

void  UScene::SetVisibilityOfEachPrimitive(EEngineShowFlags InPrimitiveToHide, bool isOn)
{
	switch (InPrimitiveToHide)
	{
	case EEngineShowFlags::SF_Primitives:
		hidePrimitive = isOn;
		break;
	case EEngineShowFlags::SF_BillboardText:
		hideTextholder = isOn;
		break;
	}
}

void UScene::Render()
{
	// 카메라가 바뀌면 원하는 타이밍(매 프레임도 OK)에 알려주면 됨
	renderer->SetTargetAspect(camera->GetAspect());

	renderer->SetViewProj(camera->GetView(), camera->GetProj());

	// Render legacy objects (components)
	for (UObject* obj : objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			const bool isText = primitive->IsA<UTextholderComp>();
			if (hidePrimitive && !isText)
				continue;
			if (hideTextholder && isText)
				continue;

			primitive->Draw(*renderer);
		}
	}

	// Render actors
	for (AActor* actor : actors)
	{
		if (actor)
		{
			// Draw only root-level primitive components (children will be drawn by their parents)
			auto components = actor->GetComponents<UPrimitiveComponent>();
			for (UPrimitiveComponent* primitive : components)
			{
				if (primitive && !primitive->GetAttachParent())  // Only draw if no parent (root-level)
				{
					const bool isText = primitive->IsA<UTextholderComp>();
					if (hidePrimitive && !isText)
						continue;
					if (hideTextholder && isText)
						continue;

					primitive->Draw(*renderer);  // This will also draw attached children
				}
			}
		}
	}
}

void UScene::Update(float deltaTime)
{
  	renderer->GetBackBufferSize(backBufferWidth, backBufferHeight);

	if (backBufferHeight > 0)
	{
		camera->SetAspect((float)backBufferWidth / (float)backBufferHeight);
	}

	TArray<USceneComponent*> deleteTarget;
	TArray<AActor*> deleteActorTarget;

	// Update legacy components
	for (UObject* obj : objects)
	{
		if (USceneComponent* sceneComponent = obj->Cast<USceneComponent>())
		{
			sceneComponent->Update(deltaTime);

			if (sceneComponent->markedAsDestroyed)
				deleteTarget.push_back(sceneComponent);
		}
	}

	// Update actors
	for (AActor* actor : actors)
	{
		if (actor)
		{
			actor->Update(deltaTime);

			if (actor->markedAsDestroyed)
				deleteActorTarget.push_back(actor);
		}
	}

	// TODO : delete/move after test
	if (inputManager->IsKeyPressed(VK_CONTROL))
	{
		AddObject(new UTextholderComp);
	}

	// Delete legacy components
	for (USceneComponent* component : deleteTarget)
	{
		component->OnShutdown();
		USceneManager* sceneManager = UEngineStatics::GetSubsystem<USceneManager>();
		UInputManager* inputManager = UEngineStatics::GetSubsystem<UInputManager>();

		sceneManager->GetScene()->RemoveObject(component);
		inputManager->UnregisterCallbacks(std::to_string(component->InternalIndex));
		GUObjectArray.erase(std::find(GUObjectArray.begin(), GUObjectArray.end(), component));
		application->OnObjectDestroyed(component);
		delete(component);
	}

	// Delete actors
	for (AActor* actor : deleteActorTarget)
	{
		RemoveActor(actor);
	}

}

bool UScene::OnInitialize()
{

	return true;
}

void UScene::AddActor(AActor* actor)
{
	if (!actor)
		return;

	actors.push_back(actor);
	actor->Initialize();
}

void UScene::RemoveActor(AActor* actor)
{
	if (!actor)
		return;

	auto it = std::find(actors.begin(), actors.end(), actor);
	if (it != actors.end())
	{
		actor->OnShutdown();
		actors.erase(it);
		delete actor;
	}
}

