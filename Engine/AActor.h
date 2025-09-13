#pragma once
#include <type_traits>
#include "UClass.h"
#include "UEngineStatics.h"
class UActorComponent; // Forward Declaration
class USceneComponent;
class AActor : public UObject
{
	DECLARE_UCLASS(AActor, UObject)
public:
	virtual ~AActor() = default;
	AActor() = default;
	// TODO: delete copy ctor/assignment operator
	// TODO: USceneComponent* GetRootComponent()
	template<typename T, typename... TArgs>
	void AddComponentByClass(TArgs... Args)
	{
		// TODO
		TUniquePtr<UActorComponent> Component = MakeUnique<T>(std::forward(Args)...);
		Component->Owner = this;
		OwnedComponents.insert(std::move(Component));
	}
	/** @note: Same with FindComponentByClass */
	template<typename T>
	T* GetComponentByClass() const
	{
		return FindComponentByClass<T>();
	}
	template<typename T>
	T* FindComponentByClass() const
	{
		static_assert(std::is_base_of_v<UActorComponent, T>, "'T' template parameter to FindComponentByClass must be derived from UActorComponent");
		return (T*)FindComponentBylass(T::StaticClass());
	}
	UActorComponent* FindComponentByClass(const UActorComponent* ComponentClass) const;
private:
	// TODO: USceneComponent* RootComponent;
	/** @note: Actual UE uses UObjectPtr. */
	TSet<TUniquePtr<UActorComponent>> OwnedComponents;
};

IMPLEMENT_UCLASS(AActor, UObject)