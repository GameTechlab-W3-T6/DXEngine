// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UCamera.h" 
#include "UTextureManager.h"
#include "UTextholderComp.h"

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
bool UScene::Initialize(URenderer* r, UMeshManager* mm, UInputManager* im, UTextureManager* tm)
{
	renderer = r;
	meshManager = mm;
	inputManager = im;
	textureManager = tm;

	backBufferWidth = 0.0f;
	backBufferHeight = 0.0f;

	// 모든 Primitive 컴포넌트 초기화
	for (UObject* obj : objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			primitive->Init(meshManager, inputManager, textureManager, camera);
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

void UScene::AddObject(USceneComponent* obj, bool hasText)
{
	// 런타임에서만 사용 - Scene이 Initialize된 후에 호출할 것
	assert(meshManager != nullptr && "AddObject should only be called after Scene initialization");

	if (!meshManager)
	{
		// 릴리즈 빌드에서 안전성 확보
		return;
	}
	
	if (!meshManager)
	{
		// 릴리즈 빌드에서 안전성 확보
		return;
	}

	objects.push_back(obj);

	// 일단 표준 RTTI 사용
	if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
	{
		if (hasText)
		{
			primitive->Init(meshManager, inputManager, textureManager, camera);
		}
		else
		{
			primitive->Init(meshManager, inputManager, textureManager);
		}
		if (obj->CountOnInspector())
			++primitiveCount;
	}
}

json::JSON UScene::Serialize() const
{
	json::JSON result;
	// UScene 특성에 맞는 JSON 구성
	result["Version"] = version;
	result["NextUUID"] = std::to_string(UEngineStatics::GetNextUUID());
	int32 validCount = 0;
	for (UObject* object : objects)
	{
		if (object == nullptr) continue;
		json::JSON _json = object->Serialize();
		if (!_json.IsNull())
		{
			//result["Primitives"][std::to_string(validCount)] = _json;
			result["Primitives"][std::to_string(object->UUID)] = _json;
			++validCount;
		}
	}
	return result;
}

bool UScene::Deserialize(const json::JSON& data)
{
	version = data.at("Version").ToInt();
	//nextUUID = data.at("NextUUID").ToInt();

	objects.clear();

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

	USceneComponent* gizmoGrid = new UGizmoGridComp(
		{ 0.3f, 0.3f, 0.3f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.2f, 0.2f, 0.2f }
	);
	objects.push_back(gizmoGrid);

	FString uuidStr = data.at("NextUUID").ToString();

	UEngineStatics::SetNextUUID((uint32)stoi(uuidStr));
	UEngineStatics::SetUUIDGeneration(true);

	return true;
}

void UScene::Render()
{
	// 카메라가 바뀌면 원하는 타이밍(매 프레임도 OK)에 알려주면 됨
	renderer->SetTargetAspect(camera->GetAspect());

	renderer->SetViewProj(camera->GetView(), camera->GetProj());

	for (UObject* obj : objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			if(primitive->bVisible)
				primitive->Draw(*renderer);
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

	float dx = 0, dy = 0, dz = 0;
	bool boost = inputManager->IsKeyDown(VK_SHIFT); // Shift로 가속

	// --- 마우스룩: RMB 누른 동안 회전 ---
	if (inputManager->IsMouseLooking())
	{
		// 마우스룩 모드는 WndProc에서 Begin/End로 관리
		float mdx = 0.f, mdy = 0.f;
		inputManager->ConsumeMouseDelta(mdx, mdy);

		const float sens = 0.005f; // 일단 크게 해서 동작 확인
		camera->AddYawPitch(mdx * sens, mdy * sens);
	}
	if (inputManager->IsKeyPressed(VK_CONTROL))
	{
		AddObject(new UTextholderComp, true);
	}

	static float t = 0.0f; t += deltaTime;
	// 대각선 이동 속도 보정(선택): 벡터 정규화
	float len = sqrtf(dx * dx + dy * dy + dz * dz);
	if (len > 0.f) { dx /= len; dy /= len; dz /= len; }
	camera->MoveLocal(dx, dy, dz, deltaTime, boost);
}

bool UScene::OnInitialize()
{

	return true;
}

