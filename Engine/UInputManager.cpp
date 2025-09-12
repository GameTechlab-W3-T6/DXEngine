#include "stdafx.h"
#include "UInputManager.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UInputManager, UEngineSubsystem)
UInputManager::UInputManager()
    : mouseX(0), mouseY(0)
    , prevMouseX(0), prevMouseY(0)
    , mouseDeltaX(0), mouseDeltaY(0)
    , wheelDelta(0)
    , initializedMouse(false)
    , mouseLook(false)
    , accumDX(0.0f), accumDY(0.0f)
    , hWndCapture(nullptr)
    , centerX(0), centerY(0)
    , skipNextMouseMove(false)
{
    memset(keyStates, 0, sizeof(keyStates));
    memset(prevKeyStates, 0, sizeof(prevKeyStates));
    memset(mouseButtons, 0, sizeof(mouseButtons));
    memset(prevMouseButtons, 0, sizeof(prevMouseButtons));
}

UInputManager::~UInputManager()
{
    // 마우스룩 모드가 활성화되어 있으면 정리
    SafeEndMouseLook();
}

void UInputManager::Update()
{
    // Copy current states to previous states for next frame comparison
    memcpy(prevKeyStates, keyStates, sizeof(keyStates));
    memcpy(prevMouseButtons, mouseButtons, sizeof(mouseButtons));

    // Prepare per-frame deltas
    prevMouseX = mouseX;
    prevMouseY = mouseY;
    mouseDeltaX = 0;
    mouseDeltaY = 0;
    wheelDelta = 0;
}

bool UInputManager::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        HandleKeyDown(wParam);
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        HandleKeyUp(wParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        HandleMouseButton(msg, wParam);
        break;
        // Fall through to handle mouse position
    // === RMB로 마우스룩 진입 ===
    case WM_RBUTTONDOWN:
        {
            HandleMouseButton(msg, wParam);  // ← 추가!!
            SetCapture(hWnd);
            hWndCapture = hWnd;

            // 화면 중앙 좌표 계산
            RECT rect;
            GetClientRect(hWnd, &rect);
            centerX = rect.right / 2;
            centerY = rect.bottom / 2;
            
            // 커서를 화면 중앙으로 이동
            POINT centerPoint = { centerX, centerY };
            ClientToScreen(hWnd, &centerPoint);
            SetCursorPos(centerPoint.x, centerPoint.y);

            // 커서 숨김(짝 맞춰 호출!)
            ShowCursor(FALSE);

            // ★ 여기서 인스턴스를 통해 접근해야 함
            BeginMouseLook();
            break;
        }
    // === RMB 해제 ===
    case WM_RBUTTONUP:
        HandleMouseButton(msg, wParam);
        SafeEndMouseLook();
        break;

    case WM_MOUSEMOVE:
        HandleMouseMove(lParam);
        break;

    case WM_MOUSEWHEEL:
        HandleMouseWheel(wParam);
        break;
        // 포커스 잃으면 안전 해제
    case WM_KILLFOCUS:
        SafeEndMouseLook();
        break;
    }

    return false; // 보통 0 반환(핸들드)해도 되지만, 기본 처리가 필요 없으면 true/0 선택
}

bool UInputManager::IsKeyDown(int32 keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    return keyStates[keyCode];
} 

bool UInputManager::IsKeyPressed(int32 keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    // Key is pressed this frame but was not pressed last frame
    return keyStates[keyCode] && !prevKeyStates[keyCode];
}

bool UInputManager::IsKeyReleased(int32 keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    // Key was pressed last frame but is not pressed this frame
    return !keyStates[keyCode] && prevKeyStates[keyCode];
}

bool UInputManager::IsMouseButtonDown(int32 button) const
{
    if (button < 0 || button >= 3)
        return false;

    return mouseButtons[button];
}

bool UInputManager::IsMouseButtonPressed(int32 button) const
{
    if (button < 0 || button >= 3)
        return false;

    // Button is pressed this frame but was not pressed last frame
    return mouseButtons[button] && !prevMouseButtons[button];
}

bool UInputManager::IsMouseButtonReleased(int32 button) const
{
    if (button < 0 || button >= 3)
        return false;

    // Button was pressed last frame but is not pressed this frame
    return !mouseButtons[button] && prevMouseButtons[button];
}

void UInputManager::ResetStates()
{
    // 마우스룩 모드가 활성화되어 있으면 먼저 안전하게 종료
    SafeEndMouseLook();

    // Clear all input states
    memset(keyStates, 0, sizeof(keyStates));
    memset(prevKeyStates, 0, sizeof(prevKeyStates));
    memset(mouseButtons, 0, sizeof(mouseButtons));
    memset(prevMouseButtons, 0, sizeof(prevMouseButtons));

    mouseX = mouseY = prevMouseX = prevMouseY = 0;
    mouseDeltaX = mouseDeltaY = 0;
    wheelDelta = 0;
    initializedMouse = false;
}

void UInputManager::HandleKeyDown(WPARAM wParam)
{
    if (wParam < 256)
    {
        for (const auto& keyCallback : keyCallbacks)
        {
            if(!keyStates[wParam])
                keyCallback.second(wParam);
        }

        keyStates[wParam] = true;
    }
}

void UInputManager::HandleKeyUp(WPARAM wParam)
{
    if (wParam < 256)
    {
        keyStates[wParam] = false;
    }
}

void UInputManager::HandleMouseMove(LPARAM lParam)
{
    //int32 newMouseX = LOWORD(lParam);
    //int32 newMouseY = HIWORD(lParam);
    // 부호를 살리기 위해서 short로 캐스팅 한다고 함
    int32 x = static_cast<short>(LOWORD(lParam));
    int32 y = static_cast<short>(HIWORD(lParam));

    if (!initializedMouse) {
        prevMouseX = mouseX = x;
        prevMouseY = mouseY = y;
        initializedMouse = true;
        return;
    }

    // 무한 마우스 회전을 위한 센터링 처리
    if (mouseLook) {
        // 중앙에서의 델타를 계산
        int32 dx = x - centerX;
        int32 dy = y - centerY;
        
        // 델타가 있을 때만 처리
        if (dx != 0 || dy != 0) {
            // 회전 델타 누적
            accumDX += static_cast<float>(dx);
            accumDY += static_cast<float>(dy);
            
            // 커서를 다시 중앙으로 이동 (더 안전한 방식)
            try {
                if (hWndCapture && IsWindow(hWndCapture)) {
                    POINT centerPoint = { centerX, centerY };
                    if (ClientToScreen(hWndCapture, &centerPoint)) {
                        SetCursorPos(centerPoint.x, centerPoint.y);
                    }
                }
            }
            catch (...) {
                // 예외 발생 시 마우스룩 모드를 안전하게 종료
                mouseLook = false;
                hWndCapture = nullptr;
                ShowCursor(TRUE);
                if (GetCapture()) {
                    ReleaseCapture();
                }
            }
        }
        
        // 마우스 위치를 중앙으로 업데이트
        mouseX = centerX;
        mouseY = centerY;
        return;
    }

    int32 dx = x - mouseX;
    int32 dy = y - mouseY;

    mouseX = x; mouseY = y;

    // 기존 누적(혹시 다른 곳에서 쓰면 계속 유지)
    mouseDeltaX += dx;
    mouseDeltaY += dy;
}

void UInputManager::HandleMouseButton(UINT message, WPARAM wParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        mouseButtons[0] = true;
        break;
    case WM_LBUTTONUP:
        mouseButtons[0] = false;
        break;

    case WM_RBUTTONDOWN:
        mouseButtons[1] = true;
        break;
    case WM_RBUTTONUP:
        mouseButtons[1] = false;
        break;

    case WM_MBUTTONDOWN:
        mouseButtons[2] = true;
        break;
    case WM_MBUTTONUP:
        mouseButtons[2] = false;
        break;
    }
}

void UInputManager::HandleMouseWheel(WPARAM wParam)
{
    // Get wheel delta (positive = forward, negative = backward)
    wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
}

void UInputManager::SafeEndMouseLook()
{
    if (!mouseLook) return;

    try {
        if (hWndCapture && IsWindow(hWndCapture)) {
            if (GetCapture() == hWndCapture) {
                ReleaseCapture();
            }
            ShowCursor(TRUE);
        }
    }
    catch (...) {
        // Even if exception occurs, ensure we reset our state
    }

    // Always reset our internal state regardless of Windows API success
    mouseLook = false;
    hWndCapture = nullptr;
    accumDX = accumDY = 0.0f;
}

void UInputManager::Shutdown()
{
    // 마우스룩 모드 안전 종료
    SafeEndMouseLook();
    
    // 모든 입력 상태 초기화
    ResetStates();
    
}

// 콜백 시스템 구현
void UInputManager::RegisterKeyCallback(const FString& id, KeyCallback callback)
{
    keyCallbacks[id] = callback;
}

void UInputManager::RegisterMouseCallback(const FString& id, MouseCallback callback)
{
    mouseCallbacks[id] = callback;
}

void UInputManager::UnregisterCallbacks(const FString& id)
{
    keyCallbacks.erase(id);
    mouseCallbacks.erase(id);
}

void UInputManager::UnregisterAllCallbacks()
{
    keyCallbacks.clear();
    mouseCallbacks.clear();
}