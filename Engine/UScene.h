#pragma once
#include "ISerializable.h"
#include "TArray.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"
#include "UGizmoManager.h"
#include "AActor.h"

class UCamera;
class URaycastManager;

class UScene : public UObject
{
	DECLARE_UCLASS(UScene, UObject)
protected:
	int32 backBufferWidth, backBufferHeight;
	int32 version;
	int32 primitiveCount;
	bool isInitialized;
	TArray<AActor*> objects;

	// Reference from outside
	URenderer* renderer;
	UMeshManager* meshManager;
	UInputManager* inputManager;
	//URaycastManager* RaycastManager;
	//UGizmoManager* GizmoManager;

	//UScene owns camera
	UCamera* camera;

	virtual void RenderGUI() {}
	virtual void OnShutdown() {}
public:
	UScene();
	virtual ~UScene();
	virtual bool Initialize(URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

	virtual void Render();
	virtual void Update(float deltaTime);
	virtual bool OnInitialize();

	bool IsInitialized() { return isInitialized; }

	int32 GetObjectCount() { return primitiveCount; }

	static UScene* Create(json::JSON data);

	void AddObject(AActor* obj);
	void SetVersion(int32 v) { version = v; }

	json::JSON Serialize() const override;

	bool Deserialize(const json::JSON& data) override;

	const TArray<AActor*>& GetObjects() const { return objects; }
	UCamera* GetCamera() { return camera; }
	URenderer* GetRenderer() { return renderer; }
	UInputManager* GetInputManager() { return inputManager; }

	int32 GetBackBufferWidth() { return backBufferWidth; };
	int32 GetBackBufferHeight() { return backBufferHeight; };
};