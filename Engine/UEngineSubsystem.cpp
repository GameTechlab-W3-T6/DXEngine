#include "stdafx.h"
#include "UEngineSubsystem.h"
#include "UEngineStatics.h"
#include "UObject.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UEngineSubsystem, UObject)

UEngineSubsystem::UEngineSubsystem()
{
    // 모든 서브시스템 자동 등록
    UEngineStatics::RegisterSubsystem(this);
}

UEngineSubsystem::~UEngineSubsystem()
{
    // 모든 서브시스템 자동 해제
    UEngineStatics::UnregisterSubsystem(this);
}
