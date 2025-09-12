#include "stdafx.h"
#include "URenderer.h"
#include "UClass.h"

IMPLEMENT_UCLASS(URenderer, UEngineSubsystem)

URenderer::URenderer()
	: device(nullptr)
	, deviceContext(nullptr)
	, swapChain(nullptr)
	, renderTargetView(nullptr)
	, depthStencilView(nullptr)
	, vertexShader(nullptr)
	, pixelShader(nullptr)
	, inputLayout(nullptr)
	, constantBuffer(nullptr)
	, rasterizerState(nullptr)
	, hWnd(nullptr)
	, bIsInitialized(false)
{
	ZeroMemory(&viewport, sizeof(viewport));
}

URenderer::~URenderer()
{
	Release();
}

bool URenderer::Initialize(HWND windowHandle)
{
	if (bIsInitialized)
		return true;

	hWnd = windowHandle;

	// Create device and swap chain
	if (!CreateDeviceAndSwapChain(windowHandle))
	{
		LogError("CreateDeviceAndSwapChain", E_FAIL);
		return false;
	}

	// Create render target view
	if (!CreateRenderTargetView())
	{
		LogError("CreateRenderTargetView", E_FAIL);
		return false;
	}

	// Get back buffer size and create depth stencil view
	int32 width, height;
	GetBackBufferSize(width, height);

	if (!CreateDepthStencilView(width, height))
	{
		LogError("CreateDepthStencilView", E_FAIL);
		return false;
	}

	// Setup viewport
	if (!SetupViewport(width, height))
	{
		LogError("SetupViewport", E_FAIL);
		return false;
	}

	if (!CreateRasterizerState())
	{
		LogError("CreateRasterizerState", E_FAIL);
		return false;
	}

	bIsInitialized = true;
	return true;
}

// URenderer.cpp의 CreateShader() 함수를 다음과 같이 수정

bool URenderer::CreateShader()
{
	// Load vertex shader from file
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		L"ShaderW0.vs",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",                   // 진입점 함수명
		"vs_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&vsBlob,                  // 컴파일된 셰이더
		&errorBlob                // 에러 메시지
	);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA("Vertex Shader Compile Error:\n");
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			SAFE_RELEASE(errorBlob);
		}
		else
		{
			OutputDebugStringA("Failed to load vertex shader file: ShaderW0.vs\n");
		}
		return false;
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		nullptr, &vertexShader);
	if (!CheckResult(hr, "CreateVertexShader"))
	{
		SAFE_RELEASE(vsBlob);
		return false;
	}

	// Create input layout
	//D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0,  DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	hr = device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		&inputLayout);
	SAFE_RELEASE(vsBlob);

	if (!CheckResult(hr, "CreateInputLayout"))
	{
		return false;
	}

	// Load pixel shader from file
	ID3DBlob* psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ShaderW0.ps",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",                   // 진입점 함수명
		"ps_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&psBlob,                  // 컴파일된 셰이더
		&errorBlob                // 에러 메시지
	);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA("Pixel Shader Compile Error:\n");
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			SAFE_RELEASE(errorBlob);
		}
		else
		{
			OutputDebugStringA("Failed to load pixel shader file: ShaderW0.ps\n");
		}
		return false;
	}

	// Create pixel shader
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
		nullptr, &pixelShader);
	SAFE_RELEASE(psBlob);

	return CheckResult(hr, "CreatePixelShader");
}

bool URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;           // 뒷면 제거
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	return CheckResult(hr, "CreateRasterizerState");
}

bool URenderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(CBTransform);   // ← 변경
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
	return CheckResult(hr, "CreateConstantBuffer");
}

ID3D11Buffer* URenderer::CreateVertexBuffer(const void* data, size_t sizeInBytes)
{
	if (!device || !data || sizeInBytes == 0)
		return nullptr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeInBytes);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;

	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);

	if (FAILED(hr))
	{
		LogError("CreateVertexBuffer", hr);
		return nullptr;
	}

	return buffer;
}

bool URenderer::UpdateConstantBuffer(const void* data, size_t sizeInBytes)
{
	if (!constantBuffer || !data)
		return false;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		LogError("Map ConstantBuffer", hr);
		return false;
	}

	memcpy(mappedResource.pData, data, sizeInBytes);
	deviceContext->Unmap(constantBuffer, 0);

	return true;
}

void URenderer::LogError(const char* function, HRESULT hr)
{
	char errorMsg[512];
	sprintf_s(errorMsg, "URenderer::%s failed with HRESULT: 0x%08X", function, hr);
	OutputDebugStringA(errorMsg);
}

bool URenderer::CheckResult(HRESULT hr, const char* function)
{
	if (FAILED(hr))
	{
		LogError(function, hr);
		return false;
	}
	return true;
}

// URenderer.cpp의 나머지 함수들 (기존 코드에 추가)

void URenderer::Release()
{
	if (!bIsInitialized)
		return;

	ReleaseShader();
	ReleaseConstantBuffer();

	SAFE_RELEASE(rasterizerState);
	SAFE_RELEASE(depthStencilView);
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(swapChain);
	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(device);

	bIsInitialized = false;
	hWnd = nullptr;
}

void URenderer::ReleaseShader()
{
	SAFE_RELEASE(inputLayout);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);
}

void URenderer::ReleaseConstantBuffer()
{
	SAFE_RELEASE(constantBuffer);
}

bool URenderer::ReleaseVertexBuffer(ID3D11Buffer* buffer)
{
	if (buffer)
	{
		buffer->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseIndexBuffer(ID3D11Buffer* buffer)
{
	if (buffer)
	{
		buffer->Release();
		return true;
	}
	return false;
}

ID3D11Buffer* URenderer::CreateIndexBuffer(const void* data, size_t sizeInBytes)
{
	if (!device || !data || sizeInBytes == 0)
		return nullptr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeInBytes);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;

	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);

	if (FAILED(hr))
	{
		LogError("CreateIndexBuffer", hr);
		return nullptr;
	}

	return buffer;
}

ID3D11Texture2D* URenderer::CreateTexture2D(int32 width, int32 height, DXGI_FORMAT format, const void* data)
{
	if (!device || width <= 0 || height <= 0)
		return nullptr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<UINT>(width);
	textureDesc.Height = static_cast<UINT>(height);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
	D3D11_SUBRESOURCE_DATA initData = {};
	if (data)
	{
		initData.pSysMem = data;
		initData.SysMemPitch = width * 4; // Assuming 4 bytes per pixel (RGBA)
		pInitData = &initData;
	}

	ID3D11Texture2D* texture = nullptr;
	HRESULT hr = device->CreateTexture2D(&textureDesc, pInitData, &texture);

	if (FAILED(hr))
	{
		LogError("CreateTexture2D", hr);
		return nullptr;
	}

	return texture;
}

ID3D11ShaderResourceView* URenderer::CreateShaderResourceView(ID3D11Texture2D* texture)
{
	if (!device || !texture)
		return nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA format
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* srv = nullptr;
	HRESULT hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);

	if (FAILED(hr))
	{
		LogError("CreateShaderResourceView", hr);
		return nullptr;
	}

	return srv;
}

bool URenderer::ReleaseTexture(ID3D11Texture2D* texture)
{
	if (texture)
	{
		texture->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseShaderResourceView(ID3D11ShaderResourceView* srv)
{
	if (srv)
	{
		srv->Release();
		return true;
	}
	return false;
}

void URenderer::Prepare()
{
	if (!deviceContext)
		return;

	// Set render target and depth stencil view
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Set viewport
	deviceContext->RSSetViewports(1, &currentViewport);

	// Clear render target and depth stencil
	Clear();
}

void URenderer::PrepareShader()
{
	if (!deviceContext)
		return;

	// Set shaders
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);

	// Set input layout
	deviceContext->IASetInputLayout(inputLayout);

	// Set primitive topology (default to triangle list)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set rasterizer state (와인딩 순서 적용)
	if (rasterizerState)
	{
		deviceContext->RSSetState(rasterizerState);
	}

	// Set constant buffer
	if (constantBuffer)
	{
		deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	}
}

void URenderer::SwapBuffer()
{
	if (swapChain)
	{
		swapChain->Present(1, 0); // VSync enabled
	}
}

void URenderer::Clear(float r, float g, float b, float a)
{
	if (!deviceContext)
		return;

	float clearColor[4] = { r, g, b, a };

	if (renderTargetView)
	{
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
	}

	if (depthStencilView)
	{
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void URenderer::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	if (deviceContext)
	{
		deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
}

void URenderer::Draw(UINT vertexCount, UINT startVertexLocation)
{
	if (deviceContext)
	{
		deviceContext->Draw(vertexCount, startVertexLocation);
	}
}

void URenderer::DrawMesh(UMesh* mesh)
{
	if (!mesh || !mesh->IsInitialized())
		return;

	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
	deviceContext->IASetPrimitiveTopology(mesh->PrimitiveType);

	deviceContext->Draw(mesh->NumVertices, 0);
}

void URenderer::DrawMeshOnTop(UMesh* mesh)
{
	if (!mesh || !mesh->IsInitialized())
		return;
	
	// Create a depth-stencil state with depth testing disabled
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = FALSE;  // disable depth testing
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* pDSState = nullptr;
	HRESULT hr = device->CreateDepthStencilState(&dsDesc, &pDSState);
	if (FAILED(hr))
	{
		LogError("CreateDepthStencilState (DrawMeshOnTop)", hr);
		return;
	}

	// Backup current depth-stencil state
	ID3D11DepthStencilState* pOldState = nullptr;
	UINT stencilRef = 0;
	deviceContext->OMGetDepthStencilState(&pOldState, &stencilRef);

	// Set new state (no depth test)
	deviceContext->OMSetDepthStencilState(pDSState, 0);

	// Draw mesh
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
	deviceContext->IASetPrimitiveTopology(mesh->PrimitiveType);
	deviceContext->Draw(mesh->NumVertices, 0);

	// Restore previous depth state
	deviceContext->OMSetDepthStencilState(pOldState, stencilRef);

	// Release local COM objects
	SAFE_RELEASE(pOldState);
	SAFE_RELEASE(pDSState);
}

void URenderer::SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset)
{
	if (deviceContext && buffer)
	{
		deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	}
}

void URenderer::SetIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT format)
{
	if (deviceContext && buffer)
	{
		deviceContext->IASetIndexBuffer(buffer, format, 0);
	}
}

void URenderer::SetConstantBuffer(ID3D11Buffer* buffer, UINT slot)
{
	if (deviceContext && buffer)
	{
		deviceContext->VSSetConstantBuffers(slot, 1, &buffer);
	}
}

void URenderer::SetTexture(ID3D11ShaderResourceView* srv, UINT slot)
{
	if (deviceContext && srv)
	{
		deviceContext->PSSetShaderResources(slot, 1, &srv);
	}
}

bool URenderer::ResizeBuffers(int32 width, int32 height)
{
	if (!swapChain || width <= 0 || height <= 0)
		return false;

	// Release render target view before resizing
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(depthStencilView);

	// Resize swap chain buffers
	HRESULT hr = swapChain->ResizeBuffers(0, static_cast<UINT>(width), static_cast<UINT>(height),
		DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		LogError("ResizeBuffers", hr);
		return false;
	}

	// Recreate render target view
	if (!CreateRenderTargetView())
	{
		return false;
	}

	// Recreate depth stencil view
	if (!CreateDepthStencilView(width, height))
	{
		return false;
	}

	// Update viewport
	return SetupViewport(width, height);
}

bool URenderer::CheckDeviceState()
{
	if (!device)
		return false;

	HRESULT hr = device->GetDeviceRemovedReason();
	if (FAILED(hr))
	{
		LogError("Device Lost", hr);
		return false;
	}

	return true;
}

void URenderer::GetBackBufferSize(int32& width, int32& height)
{
	width = height = 0;

	if (!swapChain)
		return;

	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	if (SUCCEEDED(hr) && backBuffer)
	{
		D3D11_TEXTURE2D_DESC desc;
		backBuffer->GetDesc(&desc);
		width = static_cast<int32>(desc.Width);
		height = static_cast<int32>(desc.Height);
		backBuffer->Release();
	}
}

// Private helper functions

bool URenderer::CreateDeviceAndSwapChain(HWND windowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 0; // Use window size
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                    // Use default adapter
		D3D_DRIVER_TYPE_HARDWARE,   // Hardware acceleration
		nullptr,                    // No software module
		0,                          // No flags
		nullptr,                    // Use default feature levels
		0,                          // Number of feature levels
		D3D11_SDK_VERSION,          // SDK version
		&swapChainDesc,             // Swap chain description
		&swapChain,                 // Swap chain output
		&device,                    // Device output
		&featureLevel,              // Feature level output
		&deviceContext              // Device context output
	);

	return CheckResult(hr, "D3D11CreateDeviceAndSwapChain");
}

bool URenderer::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	if (FAILED(hr))
	{
		LogError("GetBuffer", hr);
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();

	return CheckResult(hr, "CreateRenderTargetView");
}

bool URenderer::CreateDepthStencilView(int32 width, int32 height)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = static_cast<UINT>(width);
	depthStencilDesc.Height = static_cast<UINT>(height);
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthStencilBuffer = nullptr;
	HRESULT hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);

	if (FAILED(hr))
	{
		LogError("CreateTexture2D (DepthStencil)", hr);
		return false;
	}

	hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
	depthStencilBuffer->Release();

	return CheckResult(hr, "CreateDepthStencilView");
}

bool URenderer::SetupViewport(int32 width, int32 height)
{
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	// 기본은 풀 윈도우
	currentViewport = viewport;
	return true;
}

void URenderer::SetViewProj(const FMatrix& V, const FMatrix& P)
{
	// row-vector 규약이면 곱셈 순서는 V*P가 아니라, 최종적으로 v*M*V*P가 되도록
	// 프레임 캐시엔 VP = V * P 저장
	mVP = V * P;
	// 여기서는 상수버퍼 업로드 안 함 (오브젝트에서 M과 합쳐서 업로드)
}

void URenderer::SetModel(const FMatrix& M, const FVector4& color, bool bIsSelected)
{
	// per-object: MVP = M * VP
	FMatrix MVP = M * mVP;
	CopyRowMajor(mCBData.MVP, MVP);
	memcpy(mCBData.MeshColor, &color, sizeof(float) * 4);
	mCBData.IsSelected = bIsSelected ? 1.0f : 0.0f;
	UpdateConstantBuffer(&mCBData, sizeof(mCBData));
}

D3D11_VIEWPORT URenderer::MakeAspectFitViewport(int32 winW, int32 winH) const
{
	D3D11_VIEWPORT vp{};
	vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;

	float wa = (winH > 0) ? (float)winW / (float)winH : targetAspect;
	if (wa > targetAspect)
	{
		vp.Height = (float)winH;
		vp.Width = vp.Height * targetAspect;
		vp.TopLeftY = 0.0f;
		vp.TopLeftX = 0.5f * (winW - vp.Width);
	}
	else
	{
		vp.Width = (float)winW;
		vp.Height = vp.Width / targetAspect;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.5f * (winH - vp.Height);
	}
	return vp;
}
