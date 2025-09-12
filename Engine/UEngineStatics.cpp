#include "stdafx.h"
#include "UEngineStatics.h"
#include "UEngineSubsystem.h"
#include "UClass.h"

uint32 UEngineStatics::NextUUID = 1;
uint32 UEngineStatics::TotalAllocationBytes = 0;
uint32 UEngineStatics::TotalAllocationCount = 0;
bool UEngineStatics::isEnabled = true;

// 서브시스템 레지스트리
static TArray<UEngineSubsystem*> RegisteredSubsystems;

void UEngineStatics::RegisterSubsystem(UEngineSubsystem* subsystem)
{
    if (!subsystem) return;
    
    // 중복 등록 방지
    for (UEngineSubsystem* existing : RegisteredSubsystems)
    {
        if (existing == subsystem) return;
    }
    
    RegisteredSubsystems.push_back(subsystem);
}

void UEngineStatics::UnregisterSubsystem(UEngineSubsystem* subsystem)
{
    if (!subsystem) return;
    
    auto it = std::find(RegisteredSubsystems.begin(), RegisteredSubsystems.end(), subsystem);
    if (it != RegisteredSubsystems.end())
    {
        RegisteredSubsystems.erase(it);
    }
}

UEngineSubsystem* UEngineStatics::FindSubsystemByClass(UClass* targetClass)
{
    if (!targetClass) return nullptr;
    
    for (UEngineSubsystem* subsystem : RegisteredSubsystems)
    {
        if (!subsystem) continue;
        
        // RTTI 시스템을 사용하여 타입 확인
        if (subsystem->GetClass()->IsChildOrSelfOf(targetClass))
        {
            return subsystem;
        }
    }
    
    return nullptr;
}

void UEngineStatics::ShutdownAllSubsystems()
{
    // 역순으로 종료 (의존성 고려)
    for (auto it = RegisteredSubsystems.rbegin(); it != RegisteredSubsystems.rend(); ++it)
    {
        UEngineSubsystem* subsystem = *it;
        if (subsystem)
        {
            subsystem->Shutdown();
        }
    }
    
    RegisteredSubsystems.clear();
}