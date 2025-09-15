#include "stdafx.h"
#include "URenderer.h"
#include "UClass.h"
#include "UPrimitiveComponent.h"

IMPLEMENT_UCLASS(URenderer, UEngineSubsystem)

URenderer::URenderer()
	: Device(nullptr)
	, DeviceContext(nullptr)
	, SwapChain(nullptr)
	, RenderTargetView(nullptr)
	, DepthStencilView(nullptr)
	, VertexShader(nullptr)
	, PixelShader(nullptr)
	, InputLayout(nullptr)
	, ConstantBuffer(nullptr)
	, RasterizerStateSolid(nullptr)
	, RasterizerStateWireFrame(nullptr)
	, hWnd(nullptr)
	, bIsInitialized(false)
	, DrawCallCount(0)
{
	ZeroMemory(&Viewport, sizeof(Viewport));
}

URenderer::~URenderer()
{
	Release();
}

// ==========================================================================
// Renderer Core (Resource Managment)

bool URenderer::Initialize(HWND InhWnd)
{
	if (bIsInitialized)
		return true;

	hWnd = InhWnd;

	// Create device and swap chain
	if (!CreateDeviceAndSwapChain(hWnd))
	{
		LogError(E_FAIL, "CreateDeviceAndSwapChain");
		return false;
	}

	// Create render target view
	if (!CreateRenderTargetView())
	{
		LogError(E_FAIL, "CreateRenderTargetView");
		return false;
	}

	// Get back buffer size and create depth stencil view
	int32 Width, Height;
	GetBackBufferSize(Width, Height);

	if (!CreateDepthStencilView(Width, Height))
	{
		LogError(E_FAIL, "CreateDepthStencilView");
		return false;
	}

	// Setup viewport
	if (!SetupViewport(Width, Height))
	{
		LogError(E_FAIL, "SetupViewport");
		return false;
	}

	if (!CreateRasterizerState())
	{
		LogError(E_FAIL, "CreateRasterizerState");
		return false;
	}

	bIsInitialized = true;
	return true;
}

bool URenderer::CreateShader()
{
	// Load vertex shader from file
	ID3DBlob* VertexShaderBlob = nullptr;
	ID3DBlob* ShaderErrorBlob = nullptr;

	HRESULT hResult = D3DCompileFromFile(
		L"ShaderW0.vs",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",                   // 진입점 함수명
		"vs_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&VertexShaderBlob,                  // 컴파일된 셰이더
		&ShaderErrorBlob                // 에러 메시지
	);

	if (FAILED(hResult))
	{
		if (ShaderErrorBlob)
		{
			OutputDebugStringA("Vertex Shader Compile Error:\n");
			OutputDebugStringA((char*)ShaderErrorBlob->GetBufferPointer());
			SAFE_RELEASE(ShaderErrorBlob);
		}
		else
		{
			OutputDebugStringA("Failed to load vertex shader file: ShaderW0.vs\n");
		}
		return false;
	}

	// Create vertex shader
	hResult = Device->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(),
		nullptr, &VertexShader);
	if (!CheckResult(hResult, "CreateVertexShader"))
	{
		SAFE_RELEASE(VertexShaderBlob);
		return false;
	}

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC InputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	hResult = Device->CreateInputLayout(InputElements, ARRAYSIZE(InputElements),
		VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(),
		&InputLayout);
	SAFE_RELEASE(VertexShaderBlob);

	if (!CheckResult(hResult, "CreateInputLayout"))
	{
		return false;
	}

	// Load pixel shader from file
	ID3DBlob* PixelShaderBlob = nullptr;
	hResult = D3DCompileFromFile(
		L"ShaderW0.ps",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",                   // 진입점 함수명
		"ps_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&PixelShaderBlob,                  // 컴파일된 셰이더
		&ShaderErrorBlob                // 에러 메시지
	);

	if (FAILED(hResult))
	{
		if (ShaderErrorBlob)
		{
			OutputDebugStringA("Pixel Shader Compile Error:\n");
			OutputDebugStringA((char*)ShaderErrorBlob->GetBufferPointer());
			SAFE_RELEASE(ShaderErrorBlob);
		}
		else
		{
			OutputDebugStringA("Failed to load pixel shader file: ShaderW0.ps\n");
		}
		return false;
	}

	// Create pixel shader
	hResult = Device->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(),
		nullptr, &PixelShader);
	SAFE_RELEASE(PixelShaderBlob);

	return CheckResult(hResult, "CreatePixelShader");
}

bool URenderer::CreateShader_SR()
{
	VertexShader_SR = MakeUnique<UShader>(GetDevice(), EShaderType::VertexShader, "ShaderW0.vs", "main");
	PixelShader_SR = MakeUnique<UShader>(GetDevice(), EShaderType::PixelShader, "ShaderW0.ps", "main");

	return true;
}

bool URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;           // 뒷면 제거
	RasterizerDesc.FrontCounterClockwise = FALSE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;

	HRESULT hResult = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStateSolid);
	if (!CheckResult(hResult, "CreateRasterizerState"))
	{
		return false;
	}

	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	hResult = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStateWireFrame);

	return CheckResult(hResult, "CreateRasterizerState");
}

bool URenderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = sizeof(CBTransform);   // ← 변경
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateBuffer(&BufferDesc, nullptr, &ConstantBuffer);
	return CheckResult(hr, "CreateConstantBuffer");
}

void URenderer::Release()
{
	if (!bIsInitialized)
		return;

	ReleaseShader();
	ReleaseConstantBuffer();

	SAFE_RELEASE(RasterizerStateSolid);
	SAFE_RELEASE(RasterizerStateWireFrame);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);

	bIsInitialized = false;
	hWnd = nullptr;
}

void URenderer::ReleaseShader()
{
	SAFE_RELEASE(InputLayout);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(VertexShader);
}

void URenderer::ReleaseConstantBuffer()
{
	SAFE_RELEASE(ConstantBuffer);
}

ID3D11Buffer* URenderer::CreateVertexBuffer(const void* data, size_t sizeInBytes)
{
	if (!Device || !data || sizeInBytes == 0)
		return nullptr;

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = static_cast<UINT>(sizeInBytes);
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA SubresourceData = {};
	SubresourceData.pSysMem = data;

	ID3D11Buffer* Buffer = nullptr;
	HRESULT hResult = Device->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer);

	if (FAILED(hResult))
	{
		LogError(hResult, "CreateVertexBuffer");
		return nullptr;
	}

	return Buffer;
}

ID3D11Buffer* URenderer::CreateIndexBuffer(const void* Data, size_t Size)
{
	if (!Device || !Data || Size == 0)
		return nullptr;

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = static_cast<UINT>(Size);
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA SubresourceData = {};
	SubresourceData.pSysMem = Data;

	ID3D11Buffer* Buffer = nullptr;
	HRESULT hResult = Device->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer);

	if (FAILED(hResult))
	{
		LogError(hResult, "CreateIndexBuffer");
		return nullptr;
	}

	return Buffer;
}

bool URenderer::ReleaseVertexBuffer(ID3D11Buffer* Buffer)
{
	if (Buffer)
	{
		Buffer->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseIndexBuffer(ID3D11Buffer* Buffer)
{
	if (Buffer)
	{
		Buffer->Release();
		return true;
	}
	return false;
}

ID3D11Texture2D* URenderer::CreateTexture2D(int32 Width, int32 Height, DXGI_FORMAT Format, const void* Data)
{
	if (!Device || Width <= 0 || Height <= 0)
		return nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = static_cast<UINT>(Width);
	TextureDesc.Height = static_cast<UINT>(Height);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
	D3D11_SUBRESOURCE_DATA InitData = {};
	if (Data)
	{
		InitData.pSysMem = Data;
		InitData.SysMemPitch = Width * 4; // Assuming 4 bytes per pixel (RGBA)
		pInitData = &InitData;
	}

	ID3D11Texture2D* Texture = nullptr;
	HRESULT hResult = Device->CreateTexture2D(&TextureDesc, pInitData, &Texture);

	if (FAILED(hResult))
	{
		LogError(hResult, "CreateTexture2D");
		return nullptr;
	}

	return Texture;
}

ID3D11ShaderResourceView* URenderer::CreateShaderResourceView(ID3D11Texture2D* Texture)
{
	if (!Device || !Texture)
		return nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA format
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* srv = nullptr;
	HRESULT hr = Device->CreateShaderResourceView(Texture, &srvDesc, &srv);

	if (FAILED(hr))
	{
		LogError(hr, "CreateShaderResourceView");
		return nullptr;
	}

	return srv;
}

bool URenderer::ReleaseTexture(ID3D11Texture2D* Texture)
{
	if (Texture)
	{
		Texture->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseShaderResourceView(ID3D11ShaderResourceView* ShaderResourceView)
{
	if (ShaderResourceView)
	{
		ShaderResourceView->Release();
		return true;
	}
	return false;
}

// ==========================================================================
// Rendering Features

void URenderer::Prepare()
{
	if (!DeviceContext)
		return;

	// Set render target and depth stencil view
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// Set viewport
	DeviceContext->RSSetViewports(1, &CurrentViewport);

	// Clear render target and depth stencil
	Clear();
}

void URenderer::PrepareShader(bool bIsShaderReflectionEnabled)
{
	if (!DeviceContext)
	{
		return;
	}

	if (bIsShaderReflectionEnabled)
	{
		return;
	}

	// Set shaders
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	DeviceContext->IASetInputLayout(InputLayout);

	// Set primitive topology (default to triangle list)
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set rasterizer state (와인딩 순서 적용)
	//if (RasterizerState)
	//{
	//	DeviceContext->RSSetState(RasterizerState);
	//}

	// Set constant buffer
	if (ConstantBuffer)
	{
		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	}
}

void URenderer::SwapBuffer()
{
	if (SwapChain)
	{
		SwapChain->Present(1, 0); // VSync enabled
	}
}

void URenderer::Clear(float Red, float Green, float Blue, float Alpha)
{
	if (!DeviceContext)
		return;

	float clearColor[4] = { Red, Green, Blue, Alpha };

	if (RenderTargetView)
	{
		DeviceContext->ClearRenderTargetView(RenderTargetView, clearColor);
	}

	if (DepthStencilView)
	{
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void URenderer::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	if (DeviceContext)
	{
		DeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		IncrementDrawCallCount();
	}
}

void URenderer::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	if (DeviceContext)
	{
		DeviceContext->Draw(VertexCount, StartVertexLocation);
		IncrementDrawCallCount();
	}
}

void URenderer::DrawMesh(UMesh* Mesh)
{
	if (!Mesh || !Mesh->IsInitialized())
		return;

	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Mesh->Stride, &offset);
	DeviceContext->IASetPrimitiveTopology(Mesh->PrimitiveType);

	Draw(Mesh->NumVertices, 0);
}

void URenderer::DrawLine(UMesh* Mesh)
{
	if (!Mesh || !Mesh->IsInitialized())
		return;

	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Mesh->Stride, &offset);
	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	Draw(Mesh->NumVertices, 0);
}

void URenderer::DrawMeshOnTop(UMesh* Mesh)
{
	if (!Mesh || !Mesh->IsInitialized())
		return;

	// Create a depth-stencil state with depth testing disabled
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = FALSE;  // disable depth testing
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	HRESULT hr = Device->CreateDepthStencilState(&DepthStencilDesc, &pDepthStencilState);
	if (FAILED(hr))
	{
		LogError(hr, "CreateDepthStencilState (DrawMeshOnTop)");
		return;
	}

	// Backup current depth-stencil state
	ID3D11DepthStencilState* pOldState = nullptr;
	UINT StencilRef = 0;
	DeviceContext->OMGetDepthStencilState(&pOldState, &StencilRef);

	// Set new state (no depth test)
	DeviceContext->OMSetDepthStencilState(pDepthStencilState, 0);

	// Draw mesh
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Mesh->Stride, &Offset);
	DeviceContext->IASetPrimitiveTopology(Mesh->PrimitiveType);
	Draw(Mesh->NumVertices, 0);

	// Restore previous depth state
	DeviceContext->OMSetDepthStencilState(pOldState, StencilRef);

	// Release local COM objects
	SAFE_RELEASE(pOldState);
	SAFE_RELEASE(pDepthStencilState);
}

void URenderer::SetVertexBuffer(ID3D11Buffer* Buffer, UINT Stride, UINT Offset)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->IASetVertexBuffers(0, 1, &Buffer, &Stride, &Offset);
	}
}

void URenderer::SetIndexBuffer(ID3D11Buffer* Buffer, DXGI_FORMAT Format)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->IASetIndexBuffer(Buffer, Format, 0);
	}
}

void URenderer::SetConstantBuffer(ID3D11Buffer* Buffer, UINT Slot)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->VSSetConstantBuffers(Slot, 1, &Buffer);
	}
}

void URenderer::SetTexture(ID3D11ShaderResourceView* ShaderResourceView, UINT Slot)
{
	if (DeviceContext && ShaderResourceView)
	{
		DeviceContext->PSSetShaderResources(Slot, 1, &ShaderResourceView);
	}
}

void URenderer::SetRasterizerMode(bool bIsSolid)
{
	if (bIsSolid)
	{
		DeviceContext->RSSetState(RasterizerStateSolid);
	}
	else
	{
		DeviceContext->RSSetState(RasterizerStateWireFrame);
	}
}

void URenderer::SetViewProj(const FMatrix& View, const FMatrix& Projection)
{
	// row-vector 규약이면 곱셈 순서는 V*P가 아니라, 최종적으로 v*M*V*P가 되도록
	// 프레임 캐시엔 VP = V * P 저장
	VP = View * Projection;
	// 여기서는 상수버퍼 업로드 안 함 (오브젝트에서 M과 합쳐서 업로드)
}

void URenderer::SetModel(const FMatrix& Model, const FVector4& Color, bool bIsSelected, bool bIsShaderReflectionEnabled)
{
	// per-object: MVP = M * VP
	FMatrix MVP = Model * VP;
	if (bIsShaderReflectionEnabled)
	{
		(*VertexShader_SR)["ConstantBuffer"]["MVP"] = MVP;
		(*VertexShader_SR)["ConstantBuffer"]["MeshColor"] = Color;
		(*VertexShader_SR)["ConstantBuffer"]["IsSelected"] = bIsSelected;

		/** @brief: For now, binding should be done here. You should designate cbuffer name here.*/
		VertexShader_SR->Bind(GetDeviceContext(), "ConstantBuffer");
		PixelShader_SR->Bind(GetDeviceContext());
	}
	else
	{
		CopyRowMajor(MCBData.MVP, MVP);
		memcpy(MCBData.MeshColor, &Color, sizeof(float) * 4);
		MCBData.IsSelected = bIsSelected ? 1.0f : 0.0f;
		UpdateConstantBuffer(&MCBData, sizeof(MCBData));
	}
}

bool URenderer::UpdateConstantBuffer(const void* Data, size_t Size)
{
	if (!ConstantBuffer || !Data)
		return false;

	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	HRESULT hResult = DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource);

	if (FAILED(hResult))
	{
		LogError(hResult, "Map ConstantBuffer");
		return false;
	}

	memcpy(MappedSubresource.pData, Data, Size);
	DeviceContext->Unmap(ConstantBuffer, 0);

	return true;
}

bool URenderer::ResizeBuffers(int32 Width, int32 Height)
{
	if (!SwapChain || Width <= 0 || Height <= 0)
		return false;

	// Release render target view before resizing
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DepthStencilView);

	// Resize swap chain buffers
	HRESULT hResult = SwapChain->ResizeBuffers(0, static_cast<UINT>(Width), static_cast<UINT>(Height),
		DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hResult))
	{
		LogError(hResult, "ResizeBuffers");
		return false;
	}

	// Recreate render target view
	if (!CreateRenderTargetView())
	{
		return false;
	}

	// Recreate depth stencil view
	if (!CreateDepthStencilView(Width, Height))
	{
		return false;
	}

	// Update viewport
	return SetupViewport(Width, Height);
}

D3D11_VIEWPORT URenderer::MakeAspectFitViewport(int32 winW, int32 winH) const
{
	D3D11_VIEWPORT vp{};
	vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;

	float wa = (winH > 0) ? (float)winW / (float)winH : TargetAspect;
	if (wa > TargetAspect)
	{
		vp.Height = (float)winH;
		vp.Width = vp.Height * TargetAspect;
		vp.TopLeftY = 0.0f;
		vp.TopLeftX = 0.5f * (winW - vp.Width);
	}
	else
	{
		vp.Width = (float)winW;
		vp.Height = vp.Width / TargetAspect;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.5f * (winH - vp.Height);
	}
	return vp;
}

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
		&SwapChain,                 // Swap chain output
		&Device,                    // Device output
		&featureLevel,              // Feature level output
		&DeviceContext              // Device context output
	);

	return CheckResult(hr, "D3D11CreateDeviceAndSwapChain");
}

bool URenderer::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	if (FAILED(hr))
	{
		LogError(hr, "GetBuffer");
		return false;
	}

	hr = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
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
	HRESULT hr = Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);

	if (FAILED(hr))
	{
		LogError(hr, "CreateTexture2D (DepthStencil)");
		return false;
	}

	hr = Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &DepthStencilView);
	depthStencilBuffer->Release();

	return CheckResult(hr, "CreateDepthStencilView");
}

bool URenderer::SetupViewport(int32 width, int32 height)
{
	Viewport.Width = static_cast<FLOAT>(width);
	Viewport.Height = static_cast<FLOAT>(height);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	// 기본은 풀 윈도우
	CurrentViewport = Viewport;
	return true;
}

// ==========================================================================
// Utility Features

bool URenderer::CheckDeviceState()
{
	if (!Device)
		return false;

	HRESULT hr = Device->GetDeviceRemovedReason();
	if (FAILED(hr))
	{
		LogError(hr, "Device Lost");
		return false;
	}

	return true;
}

void URenderer::GetBackBufferSize(int32& Width, int32& Height)
{
	Width = Height = 0;

	if (!SwapChain)
		return;

	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	if (SUCCEEDED(hr) && backBuffer)
	{
		D3D11_TEXTURE2D_DESC desc;
		backBuffer->GetDesc(&desc);
		Width = static_cast<int32>(desc.Width);
		Height = static_cast<int32>(desc.Height);
		backBuffer->Release();
	}
}

void URenderer::LogError(HRESULT hResult, const char* Function)
{
	char errorMsg[512];
	sprintf_s(errorMsg, "URenderer::%s failed with HRESULT: 0x%08X", Function, hResult);
	OutputDebugStringA(errorMsg);
}

bool URenderer::CheckResult(HRESULT hResult, const char* Function)
{
	if (FAILED(hResult))
	{
		LogError(hResult, Function);
		return false;
	}
	return true;
}
