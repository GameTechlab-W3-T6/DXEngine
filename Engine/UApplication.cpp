#include "stdafx.h"
#include "UApplication.h"
#include "UScene.h"

// Static member definitions
WCHAR UApplication::WindowClass[] = L"EngineWindowClass";
WCHAR UApplication::DefaultTitle[] = L"Engine Application";

// Global application pointer for window procedure
UApplication* g_pApplication = nullptr;

// Forward declaration for ImGui
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UApplication::UApplication()
	: hWnd(nullptr)
	, bIsRunning(false)
	, bIsInitialized(false)
	, windowTitle(DefaultTitle)
	, windowWidth(1024)
	, windowHeight(768)
{
	g_pApplication = this;
}

UApplication::~UApplication()
{
	if (bIsInitialized)
	{
		Shutdown();
	}
	g_pApplication = nullptr;
}

bool UApplication::Initialize(HINSTANCE hInstance, const std::wstring& title, int32 width, int32 height)
{
	if (bIsInitialized)
		return false;

	UClass::ResolveTypeBitsets();

	windowTitle = title;
	windowWidth = width;
	windowHeight = height;

	// Create Renderer
	renderer = MakeUnique<URenderer>();
	//renderer = MakeUnique<UBatchRenderer>();

	// Create main window
	if (!CreateMainWindow(hInstance))
	{
		return false;
	}

	// Initialize core systems
	if (!timeManager.Initialize(60))
	{
		MessageBox(hWnd, L"Failed to initialize TimeManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Initialize Renderer
	if (!GetRenderer().Initialize(hWnd))
	{
		MessageBox(hWnd, L"Failed to create D3D11 device and swap chain", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!GetRenderer().CreateShader())
	{
		MessageBox(hWnd, L"Failed to create shaders", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!GetRenderer().CreateShader_SR())
	{
		MessageBox(hWnd, L"Failed to create SR shaders", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!GetRenderer().CreateConstantBuffer())
	{
		MessageBox(hWnd, L"Failed to create constant buffer", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!meshManager.Initialize(renderer.get()))
	{
		MessageBox(hWnd, L"Failed to initialize mesh manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	batchShaderManager.Initialize(GetRenderer().GetDevice());

	if (!textureManager.Initialize(renderer.get()))
	{
		return false;
	}

	if (!sceneManager.Initialize(g_pApplication))
	{
		MessageBox(hWnd, L"Failed to initialize scene manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!raycastManager.Initialize(renderer.get(), &inputManager))
	{
		MessageBox(hWnd, L"Failed to initialize raycast manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!gui.Initialize(hWnd, GetRenderer().GetDevice(), GetRenderer().GetDeviceContext()))
	{
		return false;
	}

	if (!renderer->CreateInstancedVB())
	{
		return false; 
	}
	// Allow derived classes to initialize
	if (!OnInitialize())
	{
		return false;
	}

	bIsInitialized = true;
	bIsRunning = true;

	return true;
}

void UApplication::Run()
{
	if (!bIsInitialized)
		return;

	while (bIsRunning)
	{
		timeManager.BeginFrame();

		inputManager.Update();
		ProcessMessages();

		if (!bIsRunning)
			break;

		InternalUpdate();
		InternalRender();

		timeManager.EndFrame();
		timeManager.WaitForTargetFrameTime();
	}
}

void UApplication::Shutdown()
{
	if (!bIsInitialized)
		return;

	bIsRunning = false;

	// Allow derived classes to cleanup
	OnShutdown();

	// Shutdown core systems in reverse order
	inputManager.Shutdown();  // 먼저 입력 시스템 안전 종료
	gui.Shutdown();
	GetRenderer().ReleaseConstantBuffer();
	GetRenderer().ReleaseShader();
	GetRenderer().Release();

	bIsInitialized = false;
}

void UApplication::Update(float deltaTime)
{
	// Base class update - can be overridden by derived classes
	// Update core engine systems here if needed

	sceneManager.GetScene()->Update(deltaTime);
}

void UApplication::Render()
{
	// Base class render - handles GUI rendering
	// Derived classes should call this after their rendering

	sceneManager.GetScene()->Render();
}

bool UApplication::CreateMainWindow(HINSTANCE hInstance)
{
	// Register window class
	WNDCLASSW wndclass = {};
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = WindowClass;
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClassW(&wndclass))
	{
		return false;
	}

	// Calculate window size including borders
	RECT windowRect = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int32 adjustedWidth = windowRect.right - windowRect.left;
	int32 adjustedHeight = windowRect.bottom - windowRect.top;

	// Create window
	hWnd = CreateWindowExW(
		0,
		WindowClass,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		adjustedWidth, adjustedHeight,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd)
	{
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

void UApplication::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			bIsRunning = false;
			return;
		}
	}
}

void UApplication::InternalUpdate()
{
	float deltaTime = static_cast<float>(timeManager.GetDeltaTime());


	// Call derived class update
	Update(deltaTime);
}

void UApplication::InternalRender()
{
	// Prepare rendering
	GetRenderer().Prepare();

	// Call derived class render
	Render();

	GetRenderer().Draw();

	// Render GUI
	gui.BeginFrame();
	gui.Render();
	RenderGUI();
	gui.EndFrame();

	// Present the frame
	GetRenderer().SwapBuffer();
}

LRESULT CALLBACK UApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Let ImGui handle the message first
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	if (g_pApplication)
	{
		// Let input manager process input messages
		g_pApplication->inputManager.ProcessMessage(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_DESTROY:
		// 윈도우 종료 시 입력 시스템 안전 종료
		if (g_pApplication && g_pApplication->bIsInitialized)
		{
			g_pApplication->inputManager.SafeEndMouseLook();
		}
		PostQuitMessage(0);
		break;

	case WM_ENTERSIZEMOVE:
		if (g_pApplication) g_pApplication->isSizing = true;
		break;
	case WM_SIZE:
		if (g_pApplication && wParam != SIZE_MINIMIZED)
		{
			int32 width = LOWORD(lParam);
			int32 height = HIWORD(lParam);

			if (g_pApplication->isSizing)
			{
				// 드래그 중: 스왑체인 리사이즈 X, 레터/필러박스 뷰포트만 적용
				g_pApplication->windowWidth = width;
				g_pApplication->windowHeight = height;
				g_pApplication->GetRenderer().UseAspectFitViewport(width, height);
			}
			else
			{
				// 평소 리사이즈: 실제 리사이즈 + 풀윈도우 뷰포트
				g_pApplication->GetRenderer().ResizeBuffers(width, height);
				g_pApplication->GetRenderer().UseFullWindowViewport();
				g_pApplication->OnResize(width, height); // 카메라 갱신
			}
		}
		break;
	case WM_EXITSIZEMOVE:
		if (g_pApplication)
		{
			g_pApplication->isSizing = false;
			int32 width = g_pApplication->windowWidth;
			int32 height = g_pApplication->windowHeight;
			if (width > 0 && height > 0)
			{
				g_pApplication->GetRenderer().ResizeBuffers(width, height);
				g_pApplication->GetRenderer().UseFullWindowViewport();
				g_pApplication->OnResize(width, height);
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}



	return 0;
}

bool UApplication::OnInitialize()
{
	return true;

}

UScene* UApplication::CreateDefaultScene()
{
	return new UScene();
}
