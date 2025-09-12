#pragma once
#include "stdafx.h"
#include "UEngineSubsystem.h"

class UInputManager : public UEngineSubsystem
{
    DECLARE_UCLASS(UInputManager, UEngineSubsystem)
private:

    // Key states
    bool keyStates[256];
    bool prevKeyStates[256];

    // Mouse states
    bool mouseButtons[3]; // Left, Right, Middle
    bool prevMouseButtons[3];
    int32 mouseX = 0, mouseY = 0;
    int32 prevMouseX, prevMouseY;
    int32 mouseDeltaX, mouseDeltaY;

    // Per-frame wheel delta (accumulate within a frame)
    int32 wheelDelta;

    bool initializedMouse = false;
    bool mouseLook = false;
    float accumDX = 0.0f, accumDY = 0.0f; // 프레임 누적 델타
    
    // 무한 마우스 회전을 위한 변수들
    HWND hWndCapture = nullptr;
    int32 centerX = 0, centerY = 0;
    bool skipNextMouseMove = false;
public:
    UInputManager();
    ~UInputManager();

    // Frame management
    void Update();
    bool ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Keyboard input
    bool IsKeyDown(int32 keyCode) const;
    bool IsKeyPressed(int32 keyCode) const;  // True only on the frame key was pressed
    bool IsKeyReleased(int32 keyCode) const; // True only on the frame key was released
     

    // Mouse input
    bool IsMouseButtonDown(int32 button) const; // 0=Left, 1=Right, 2=Middle
    bool IsMouseButtonPressed(int32 button) const;
    bool IsMouseButtonReleased(int32 button) const;

    // Mouse position and movement
    int32 GetMouseX() const { return mouseX; }
    int32 GetMouseY() const { return mouseY; }
    // Utility
    void ResetStates();
    void SafeEndMouseLook(); // 안전한 마우스룩 종료
    void Shutdown(); // 안전한 시스템 종료


    void OnMouseMove(int32 x, int32 y) {
        if (!initializedMouse) { prevMouseX = mouseX = x; prevMouseY = mouseY = y; initializedMouse = true; }
        prevMouseX = mouseX; prevMouseY = mouseY;
        mouseX = x; mouseY = y;
        if (mouseLook) { accumDX += float(mouseX - prevMouseX); accumDY += float(mouseY - prevMouseY); }
    }

    // RMB 눌렀을 때 호출
    void BeginMouseLook() {
        mouseLook = true;
        accumDX = accumDY = 0.0f; // 첫 프레임 점프 방지
    }
    // RMB 뗐을 때 호출
    void EndMouseLook() {
        mouseLook = false;
        accumDX = accumDY = 0.0f;
        hWndCapture = nullptr;
    }
    bool IsMouseLooking() const { return mouseLook; }

    // 한 프레임치 델타를 꺼내고 0으로 리셋
    void ConsumeMouseDelta(float& dx, float& dy) {
        dx = accumDX; dy = accumDY;
        accumDX = accumDY = 0.0f;
    }

    // 개선된 콜백 시스템 - ID 기반
    using KeyCallback = TFunction<void(int32)>;
    using MouseCallback = TFunction<void(int32, int32, int32)>; // button, x, y
    
    void RegisterKeyCallback(const FString& id, KeyCallback callback);
    void RegisterMouseCallback(const FString& id, MouseCallback callback);
    void UnregisterCallbacks(const FString& id);
    void UnregisterAllCallbacks();

private:
    TMap<FString, KeyCallback> keyCallbacks;
    TMap<FString, MouseCallback> mouseCallbacks;
private:
    void HandleKeyDown(WPARAM wParam);
    void HandleKeyUp(WPARAM wParam);
    void HandleMouseMove(LPARAM lParam);
    void HandleMouseButton(UINT message, WPARAM wParam);
    void HandleMouseWheel(WPARAM wParam);
};