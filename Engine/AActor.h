#pragma once
#include "UObject.h"
#include "UEngineStatics.h"
#include "UActorComponent.h"
class USceneComponent;

class AActor : public UObject
{
	DECLARE_UCLASS(AActor, UObject)
public:
	AActor();

	template <typename T>
	T* AddComponent(TUniquePtr<T> component)
	{
		static_assert(std::is_base_of_v<UActorComponent, T>, "T must be a UActorComponent");

		if (!component)
			return nullptr;

		T* componentPtr = component.get();
		componentPtr->SetOwner(this);

		Components.push_back(std::move(component));

		// If it's a scene component and we don't have a root, make it root
		if (auto sceneComp = componentPtr->template Cast<USceneComponent>())
		{
			if (!RootComponent)
			{
				SetRootComponent(sceneComp);
			}
		}

		return componentPtr;
	}

	template <typename T>
	T* GetComponent()
	{
		static_assert(std::is_base_of_v<UActorComponent, T>, "T must be a UActorComponent");
		for (auto& comp : Components)
		{
			if (comp && comp->IsA<T>())
				return comp->Cast<T>();
		}
		return nullptr;
	}

	template <typename T>
	TArray<T*> GetComponents()
	{
		static_assert(std::is_base_of_v<UActorComponent, T>, "T must be a UActorComponent");
		TArray<T*> result;
		for (auto& comp : Components)
		{
			if (comp && comp->IsA<T>())
			{
				if (auto casted = comp->Cast<T>())
					result.push_back(casted);
			}
		}
		return result;
	}

	// Scene component management
	USceneComponent* GetRootComponent() const { return RootComponent; }
	void SetRootComponent(USceneComponent* InRootComponent);

	virtual bool Initialize();
	virtual void Update(float deltaTime);
	virtual void OnShutdown();

	// Serialization
	virtual json::JSON Serialize() const override;
	virtual bool Deserialize(const json::JSON& data) override;
	virtual uint32 GetID() const { return ID; }

	bool markedAsDestroyed = false;

private:
	static inline uint32 ActorID;
	TArray<TUniquePtr<UActorComponent>> Components;
	USceneComponent* RootComponent = nullptr;
	uint32 ID;
};