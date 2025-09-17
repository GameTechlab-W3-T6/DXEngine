#pragma once
#include "ISerializable.h"
#include "TArray.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"
#include "UGizmoManager.h"
#include "Constant.h"

class UCamera;
class URaycastManager;
class AActor;

/**
 * @brief Container for all scene objects with rendering and update functionality
 */
class UScene : public UObject
{
	DECLARE_UCLASS(UScene, UObject)
protected:
	int32 backBufferWidth, backBufferHeight;
	int32 version;
	int32 primitiveCount;
	bool isInitialized;

	TArray<USceneComponent*> objects;  // TODO: Deprecated, use actors instead
	TArray<AActor*> actors;  // New actor-based management

	// Reference from outside
	UApplication* application;
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
	virtual bool Initialize(UApplication* app, URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

	virtual void Render();
	virtual void Update(float deltaTime);
	virtual bool OnInitialize();

	bool IsInitialized() { return isInitialized; }

	int32 GetObjectCount() { return primitiveCount; }

	static UScene* Create(json::JSON data);

	void AddObject(USceneComponent* obj);  // TODO: Deprecated
	void RemoveObject(USceneComponent* obj);  // TODO: Deprecated

	// New actor-based methods
	void AddActor(AActor* actor);
	void RemoveActor(AActor* actor);

	void SetVersion(int32 v) { version = v; }

	json::JSON Serialize() const override;

	bool Deserialize(const json::JSON& data) override;

	const TArray<USceneComponent*>& GetObjects() const { return objects; }  // TODO: Deprecated
	const TArray<AActor*>& GetActors() const { return actors; }  // New method

	UCamera* GetCamera() { return camera; }
	URenderer* GetRenderer() { return renderer; }
	UInputManager* GetInputManager() { return inputManager; }

	int32 GetBackBufferWidth() { return backBufferWidth; };
	int32 GetBackBufferHeight() { return backBufferHeight; };

	bool hidePrimitive = false;
	bool hideTextholder = false;

	void SetVisibilityOfEachPrimitive(EEngineShowFlags InPrimitiveToHide, bool isOn);
};