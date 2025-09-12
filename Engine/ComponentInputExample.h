#pragma once
// Example: 컴포넌트에서 입력 콜백을 사용하는 방법

#include "USceneComponent.h"
#include "UEngineStatics.h"
#include "UInputManager.h"

class UExampleComponent : public USceneComponent
{
    DECLARE_UCLASS(UExampleComponent, USceneComponent)
    
private:
    FString componentId;
    
public:
    UExampleComponent() {
        // 고유 ID 생성 (UUID 기반)
        componentId = "ExampleComponent_" + std::to_string(UEngineStatics::GenUUID());
    }
    
    ~UExampleComponent() {
        UnregisterInputCallbacks();
    }
    
    // 컴포넌트 활성화 시 콜백 등록
    void RegisterInputCallbacks() {
        auto* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
        if (!inputManager) return;
        
        // 키 입력 콜백 등록
        inputManager->RegisterKeyCallback(componentId, 
            [this](int32 key) { OnKeyInput(key); });
        
        // 마우스 입력 콜백 등록  
        inputManager->RegisterMouseCallback(componentId,
            [this](int32 button, int32 x, int32 y) { OnMouseInput(button, x, y); });
    }
    
    // 컴포넌트 비활성화 시 콜백 해제
    void UnregisterInputCallbacks() {
        auto* inputManager = UEngineStatics::GetSubsystem<UInputManager>();
        if (inputManager) {
            inputManager->UnregisterCallbacks(componentId);
        }
    }
    
private:
    void OnKeyInput(int32 key) {
        // 키 입력 처리 로직
        if (key == 'G') {
            // G키 눌림 처리
        }
    }
    
    void OnMouseInput(int32 button, int32 x, int32 y) {
        // 마우스 입력 처리 로직
        if (button == 0) { // Left click
            // 마우스 클릭 처리
        }
    }
};

// 사용법:
// 1. 컴포넌트 생성 후 RegisterInputCallbacks() 호출
// 2. 컴포넌트 소멸 시 자동으로 UnregisterInputCallbacks() 호출됨
// 3. RTTI 시스템을 통해 타입 안전하게 서브시스템 접근