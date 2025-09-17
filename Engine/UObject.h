#pragma once
#include <type_traits>
#include "UEngineStatics.h"
#include "TArray.h"
#include "ISerializable.h"
#include "UObjectMacros.h"
#include "FName.h"

typedef int int32;
typedef unsigned int uint32;

/**
 * @brief Base class for all engine objects with serialization and object tracking
 */
class UObject : public ISerializable
{
    DECLARE_ROOT_UCLASS(UObject)
private:
    static inline TArray<uint32> FreeIndices;
    static inline uint32 NextFreshIndex = 0;
public:
    static inline TArray<UObject*> GUObjectArray;
    uint32 UUID;
    uint32 InternalIndex;
    FName name;

    static void AddTrackedObject(UObject* obj)
    {
        // 방법 1: 빠른 추가 - 재사용 인덱스 우선 사용
        if (!FreeIndices.empty())
        {
            // 삭제된 슬롯 재사용 (O(1))
            obj->InternalIndex = FreeIndices.back();
            FreeIndices.pop_back();
            GUObjectArray[obj->InternalIndex] = obj;
        }
        else
        {
            // 새 슬롯 할당 (O(1) amortized)
            obj->InternalIndex = NextFreshIndex++;
            GUObjectArray.push_back(obj);
        }
    }

    static void RemoveTrackedObject(UObject* obj)
    {
        // 방법 1: 빠른 삭제 - nullptr 마킹 + 인덱스 재사용 큐에 추가
        if (obj->InternalIndex < GUObjectArray.size() && GUObjectArray[obj->InternalIndex] == obj)
        {
            GUObjectArray[obj->InternalIndex] = nullptr;  // O(1)
            FreeIndices.push_back(obj->InternalIndex);    // O(1)
        }
    }

    UObject()
    {
        UUID = UINT_MAX;
        name = FName("New Object");
        AddTrackedObject(this);
    }

    virtual ~UObject()
    {
        RemoveTrackedObject(this);
    }

    virtual bool CountOnInspector() {
        return false;
    }

    template<typename T>
    bool IsA() const {
        return GetClass()->IsChildOrSelfOf(T::StaticClass());
    }

    template<typename T>
    T* Cast() {
        return IsA<T>() ? static_cast<T*>(this) : nullptr;
    }

    template<typename T>
    const T* Cast() const {
        return IsA<T>() ? static_cast<const T*>(this) : nullptr;
    }
    // Override new/delete for tracking
    void* operator new(size_t size)
    {
        UEngineStatics::AddAllocation(size);
        return ::operator new(size);
    }

    void operator delete(void* ptr, size_t size)
    {
        UEngineStatics::RemoveAllocation(size);
        ::operator delete(ptr);
    }

    // 배치 new/delete도 오버라이드 (필요한 경우)
    void* operator new(size_t size, void* ptr)
    {
        return ptr;  // placement new는 메모리 추적 안함
    }

    void operator delete(void* ptr, void* place)
    {
        // placement delete는 아무것도 안함
    }

    json::JSON Serialize() const override
    {
        return json::JSON();
    }

    bool Deserialize(const json::JSON& data) override
    {
        return true;
    }

    void SetUUID(uint32 uuid)
    {
        UUID = uuid;
    }

    static void ClearFreeIndices()
    {
        FreeIndices.clear();
    }
};