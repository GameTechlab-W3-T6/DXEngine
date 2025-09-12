#pragma once

// Forward declarations - 순환 참조 방지
class UClass;
class UObject;

// Root UClass 매크로 - UObject를 위한 매크로
#define DECLARE_ROOT_UCLASS(ClassName) \
public: \
    virtual UClass* GetClass() const; \
    static UClass* StaticClass(); \
private: \
    static UClass* s_StaticClass; \
    static UObject* CreateInstance();

#define IMPLEMENT_ROOT_UCLASS(ClassName) \
UObject* ClassName::CreateInstance() { return new ClassName(); } \
UClass* ClassName::s_StaticClass = UClass::RegisterToFactory( \
    #ClassName, &ClassName::CreateInstance, ""); \
UClass* ClassName::StaticClass() { return s_StaticClass; } \
UClass* ClassName::GetClass() const { return StaticClass(); }

// 일반 UClass 매크로 - 상속받는 클래스들을 위한 매크로
#define DECLARE_UCLASS(ClassName, ParentClass) \
public: \
    using Super = ParentClass; \
    UClass* GetClass() const override; \
    static UClass* StaticClass(); \
private: \
    static UClass* s_StaticClass; \
    static UObject* CreateInstance();

#define IMPLEMENT_UCLASS(ClassName, ParentClass) \
UObject* ClassName::CreateInstance() { return new ClassName(); } \
UClass* ClassName::s_StaticClass = UClass::RegisterToFactory( \
    #ClassName, &ClassName::CreateInstance, #ParentClass); \
UClass* ClassName::StaticClass() { return s_StaticClass; } \
UClass* ClassName::GetClass() const { return StaticClass(); }

// 메타데이터 매크로
#define UCLASS_META(ClassName, Key, Value) \
struct _MetaRegister_##ClassName##_##Key { \
    _MetaRegister_##ClassName##_##Key() { ClassName::StaticClass()->SetMeta(#Key, Value); } \
} _MetaRegisterInstance_##ClassName##_##Key;