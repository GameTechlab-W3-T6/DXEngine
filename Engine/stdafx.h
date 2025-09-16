#pragma once

// Windows 관련 (가장 먼저)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>


// C++ 표준 라이브러리
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>
#include <cassert>
#include <cmath>
#include <optional>
#include <filesystem>
#include <climits>
#include <bitset>
#include <stack>

// DirectX 관련
#include <d3d11.h>
#include <d3dcompiler.h>

//#include <DirectXMath.h>

// ImGui 관련 (DirectX 이후에)
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImguiConsole.h"

// 라이브러리 링킹
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Engine forward declarations
class UApplication;
class URenderer;
class UInputManager;
class UGUI;
class UTimeManager;

// Common macros
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete[] p; p = nullptr; }

const float PI = 3.14159265358979323846f;
const float PIDIV4 = PI / 4.0f;   // XM_PIDIV4 대체

const float DegreeToRadian = PI / 180.0f;

// Engine namespace (optional)
namespace Engine
{
	// Common engine types and utilities can go here
}