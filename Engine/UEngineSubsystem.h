#pragma once
#include "UObject.h"
#include "UClass.h"

/**
 * @brief Base class for engine subsystems providing core functionality
 */
class UEngineSubsystem : public UObject
{
    DECLARE_UCLASS(UEngineSubsystem, UObject)
public:
    UEngineSubsystem();
    virtual ~UEngineSubsystem();
    virtual bool Initialize() { return true; }  // 나중에 필요시 오버라이드
    virtual void Shutdown() {}
};

