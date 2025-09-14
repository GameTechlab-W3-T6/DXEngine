#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObjectMacros.h"
#include "UObject.h"
#include <memory>

/**
 * @brief Reflection system class for type information and object creation
 */
class UClass : public UObject
{
	DECLARE_UCLASS(UClass, UObject)
private:
	static inline TArray<TUniquePtr<UClass>> classList;
	static inline TMap<FString, uint32> nameToId;
	static inline TMap<FString, uint32> displayNameToId;
	static inline uint32 registeredCount = 0;

	TMap<FString, FString> metadata;
	uint32 typeId;
	FDynamicBitset typeBitset;
	FString className, superClassTypeName;
	UClass* superClass;
	TFunction<UObject*()> createFunction;
	bool processed = false;
public:
	static UClass* RegisterToFactory(const FString& typeName, 
		const TFunction<UObject* ()>& createFunction, const FString& superClassTypeName);

	static void ResolveTypeBitsets();
	void ResolveTypeBitset(UClass* classPtr);

	static UClass* GetClass(uint32 typeId) {
		return (typeId < classList.size()) ? classList[typeId].get() : nullptr;
	}

	static UClass* FindClass(const FString& name) {
		auto it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}

	static UClass* FindClassWithDisplayName(const FString& name)
	{
		// 1) DisplayName lookup
		auto it = displayNameToId.find(name);
		if (it != displayNameToId.end())
			return GetClass(it->second);

		// 2) className fallback
		it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}


	static const TArray<TUniquePtr<UClass>>& GetClassList()
	{
		return classList;
	}

	bool IsChildOrSelfOf(UClass* baseClass) const {
		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	const FString& GetUClassName() const { return className; }

	const FString& GetDisplayName() const
	{
		auto itr = metadata.find("DisplayName");
		if (itr != metadata.end())
		{
			return itr->second;
		}

		return GetUClassName();
	}

	void SetMeta(const FString& key, const FString& value)
	{
		metadata[key] = value;

		if (key == "DisplayName")
		{
			displayNameToId[value] = typeId;  // typeId는 인스턴스 멤버
		}
	}


	const FString& GetMeta(const FString& key) const {
		static FString empty;
		auto it = metadata.find(key);
		return (it != metadata.end()) ? it->second : empty;
	}

	UObject* CreateDefaultObject() const {
		return createFunction ? createFunction() : nullptr;
	}

};


