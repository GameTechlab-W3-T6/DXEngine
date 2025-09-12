#include "stdafx.h"
#include "UClass.h"
#include "UObject.h"

IMPLEMENT_UCLASS(UClass, UObject)
UClass* UClass::RegisterToFactory(const FString& typeName, const TFunction<UObject* ()>& createFunction, const FString& superClassTypeName)
{
	TUniquePtr<UClass> classType = MakeUnique<UClass>();

	classType->typeId = registeredCount++;
	classType->className = typeName;
	classType->superClassTypeName = superClassTypeName;
	classType->createFunction = createFunction;

	nameToId[typeName] = classType->typeId;

	UClass* rawPtr = classType.get();
	classList.push_back(std::move(classType));
	return rawPtr;
}

void UClass::ResolveTypeBitsets()
{
	// 모든 비트셋을 실제 클래스 수에 맞게 사전 할당
	size_t totalClasses = classList.size();
	for (const TUniquePtr<UClass>& _class : classList)
	{
		_class->typeBitset.Reserve(totalClasses);
	}

	for (const TUniquePtr<UClass>& _class : classList)
	{
        if (!_class->superClassTypeName.empty()) {
            auto it = nameToId.find(_class->superClassTypeName);
            _class->superClass = (it != nameToId.end()) ? classList[it->second].get() : nullptr;
        }
	}
    for (const TUniquePtr<UClass>& _class : classList)
    {
        if (_class->processed) continue;

        _class->ResolveTypeBitset(_class.get());
    }
}

void UClass::ResolveTypeBitset(UClass* classPtr)
{
    TArray<UClass*> stack;
    stack.push_back(classPtr);

    while (!stack.empty())
    {
        UClass* cur = stack.back();

        // 부모가 아직 처리되지 않았다면 먼저 스택에 push
        while (cur->superClass && !cur->superClass->processed)
        {
            stack.push_back(cur->superClass);
            cur = stack.back();
        }

        // 현재 노드 처리
        cur->typeBitset.Clear();
        if (cur->superClass) cur->typeBitset |= cur->superClass->typeBitset;
        cur->typeBitset.Set(cur->typeId);  // 자신의 비트 추가
        cur->processed = true;

        stack.pop_back();
    }
}