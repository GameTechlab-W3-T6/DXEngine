#include "stdafx.h"
#include "URenderer.h"
#include "UClass.h"
#include "ConfigManager.h"
#include "UPrimitiveComponent.h"
#include "FTextInfo.h"
#include "UTextureManager.h"

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
	, InputLayoutTextInst(nullptr)
	, constantBuffer(nullptr)
	, textConstantBuffer(nullptr)
	, aabbLineVB(nullptr)
	, rasterizerState_solid(nullptr)
	, hWnd(nullptr)
	, bIsInitialized(false)
{
	ConfigData* config = ConfigManager::GetConfig("editor");

	if (config)
		bIsShaderReflectionEnabled = config->getBool("Graphics", "ShaderReflection");

	else
		bIsShaderReflectionEnabled = false;

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
		L"ShaderW0VS.hlsl",           // 파일 경로
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 ,16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD",    0,  DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		&inputLayout);

	SAFE_RELEASE(vsBlob);

	if (!CheckResult(hr, "CreateInputLayout"))
	{
		return false;
	}

	ID3DBlob* vsBlobInst = nullptr; 
	ID3DBlob* errorBlobInst = nullptr;
	 hr = D3DCompileFromFile(
		L"TexTestVS.hlsl",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",						// 진입점 함수명
		"vs_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&vsBlobInst,                // 컴파일된 셰이더
		&errorBlobInst                // 에러 메시지
	);

	if (FAILED(hr))
	{
		if (errorBlobInst)
		{
			OutputDebugStringA("Instanced Vertex Shader Compile Error:\n");
			OutputDebugStringA((char*)errorBlobInst->GetBufferPointer());
			SAFE_RELEASE(errorBlobInst);
		}
		else
		{
			OutputDebugStringA("Failed to load Instanced vertex shader file: ShaderW0.vs\n");
		}
		return false;
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vsBlobInst->GetBufferPointer(), vsBlobInst->GetBufferSize(),
		nullptr, &textVertexShaderInst);
	if (!CheckResult(hr, "CreateInstancedVertexShader"))
	{
		SAFE_RELEASE(vsBlobInst);
		return false;
	}


	D3D11_INPUT_ELEMENT_DESC textInput[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 ,16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD",    0,  DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 


		// instanced
		{ "INST_M", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_M", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_M", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{ "INST_UV_OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{ "INST_UV_SCALE",  0, DXGI_FORMAT_R32G32_FLOAT, 1, 56, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{ "INST_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				
	}; 

	hr = device->CreateInputLayout(textInput, ARRAYSIZE(textInput),
		vsBlobInst->GetBufferPointer(), vsBlobInst->GetBufferSize(),
		&InputLayoutTextInst);
	if (!CheckResult(hr, "CreateInstanced InpuyLayout"))
	{
		DumpVSInputSignature(vsBlobInst);
		SAFE_RELEASE(vsBlobInst);
		return false;
	}
	// Load pixel shader from file
	ID3DBlob* psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ShaderW0PS.hlsl",           // 파일 경로
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

	// Load pixel shader from file
	ID3DBlob* psBlobIns = nullptr;
	hr = D3DCompileFromFile(
		L"TexTestPS.hlsl",           // 파일 경로
		nullptr,                  // 매크로 정의
		nullptr,                  // Include 핸들러
		"main",                   // 진입점 함수명
		"ps_5_0",                 // 셰이더 모델
		0,                        // 컴파일 플래그
		0,                        // 효과 플래그
		&psBlobIns,                  // 컴파일된 셰이더
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
	hr = device->CreatePixelShader(psBlobIns->GetBufferPointer(), psBlobIns->GetBufferSize(),
		nullptr, &textPixelShaderInst);

	SAFE_RELEASE(psBlobIns);


	return CheckResult(hr, "CreatePixelShader");
}

bool URenderer::CreateShader_SR()
{
	VertexShader_SR = MakeUnique<UShader>(GetDevice(), 0, EShaderType::VertexShader, std::filesystem::path("DefaultVS.hlsl"), "main");
	PixelShader_SR = MakeUnique<UShader>(GetDevice(), 1, EShaderType::PixelShader, std::filesystem::path("DefaultPS.hlsl"), "main");

	return true;
}

bool URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;       //TODO    // 뒷면 제거
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState_solid);
	if (!CheckResult(hr, "CreateRasterizerState"))
		return false;

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState_wireframe);

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

bool URenderer::CreateTextInstanceVB(UINT maxInstances)
{
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = maxInstances * sizeof(FTextInstance);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bd, 0, &textInstanceVB);

	return CheckResult(hr, "Create Text Instance");
}

void URenderer::UpdateTextInstanceVB(const TArray<FTextInstance>& instances)
{
	D3D11_MAPPED_SUBRESOURCE m{};

	deviceContext->Map(textInstanceVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, instances.data(), instances.size() * sizeof(FTextInstance));
	deviceContext->Unmap(textInstanceVB, 0);
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

bool URenderer::UpdateConstantBufferUV(const void* data, size_t sizeInBytes)
{
	if (!textConstantBuffer || !data)
		return false;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(textConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		LogError("UV textConstantBuffer", hr);
		return false;
	}

	memcpy(mappedResource.pData, data, sizeInBytes);
	deviceContext->Unmap(textConstantBuffer, 0);

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

	SAFE_RELEASE(rasterizerState_solid);
	SAFE_RELEASE(rasterizerState_wireframe);
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
	SAFE_RELEASE(textConstantBuffer);
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
	{
		return;
	}

	if (bIsShaderReflectionEnabled)
	{
		return;
	}

	// Set shaders
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);

	// Set input layout
	deviceContext->IASetInputLayout(inputLayout);

	// Set primitive topology (default to triangle list)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set constant buffer
	if (constantBuffer)
	{
		deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	}

	if (textConstantBuffer) 
	{
		deviceContext->PSSetConstantBuffers(0, 1, &textConstantBuffer);
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
	deviceContext->IASetInputLayout(inputLayout);

	if (deviceContext)
	{
		deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
}

void URenderer::Draw(UINT vertexCount, UINT startVertexLocation)
{
	deviceContext->IASetInputLayout(inputLayout);

	if (deviceContext)
	{
		deviceContext->Draw(vertexCount, startVertexLocation);
	}
}

void URenderer::DrawMesh(UMesh* mesh)
{
	if (!mesh || !mesh->IsInitialized())
		return;  
	deviceContext->IASetInputLayout(inputLayout);

	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(mesh->PrimitiveType);
	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
	
	if (mesh->IndexBuffer)
	{
		deviceContext->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(mesh->NumIndices, 0, 0);
	}
	else
	{
		deviceContext->Draw(mesh->NumVertices, 0);
	}
}

void URenderer::DrawLine(UMesh* mesh)
{
	if (!mesh || !mesh->IsInitialized())
		return;

	deviceContext->IASetInputLayout(inputLayout);

	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	deviceContext->Draw(mesh->NumVertices, 0);
}

void URenderer::DrawPrimitiveComponent(UPrimitiveComponent* component)
{
	UMesh* mesh = component->GetMesh();

	if (!mesh || !mesh->IsInitialized())
		return;

	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(mesh->PrimitiveType);
	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);

	if (mesh->IndexBuffer)
	{
		deviceContext->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(mesh->NumIndices, 0, 0);
	}
	else
	{
		deviceContext->Draw(mesh->NumVertices, 0);
	}
}

void URenderer::DrawGizmoComponent(UGizmoComponent* component, bool drawOnTop)
{
	UMesh* mesh = component->GetMesh();

	if (!mesh || !mesh->IsInitialized())
		return;

	// Create appropriate depth-stencil state based on drawOnTop parameter
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	if (drawOnTop)
	{
		// Disable depth testing for on-top rendering
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	}
	else
	{
		// Enable normal depth testing
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	}

	ID3D11DepthStencilState* pDSState = nullptr;
	HRESULT hr = device->CreateDepthStencilState(&dsDesc, &pDSState);
	if (FAILED(hr))
	{
		LogError("CreateDepthStencilState (DrawGizmoComponent)", hr);
		return;
	}

	// Backup current depth-stencil state
	ID3D11DepthStencilState* pOldState = nullptr;
	UINT stencilRef = 0;
	deviceContext->OMGetDepthStencilState(&pOldState, &stencilRef);

	// Set new depth state
	deviceContext->OMSetDepthStencilState(pDSState, 0);

	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(mesh->PrimitiveType);
	deviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);

	if (mesh->IndexBuffer)
	{
		deviceContext->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(mesh->NumIndices, 0, 0);
	}
	else
	{
		deviceContext->Draw(mesh->NumVertices, 0);
	}

	// Restore previous depth state
	deviceContext->OMSetDepthStencilState(pOldState, stencilRef);

	// Release local COM objects
	SAFE_RELEASE(pOldState);
	SAFE_RELEASE(pDSState);
}

void URenderer::DrawMeshOnTop(UMesh* mesh)
{
	if (!mesh || !mesh->IsInitialized())
		return;
	deviceContext->IASetInputLayout(inputLayout);

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

/**
 * @brief 텍스트 메시(사각형 등)를 인스턴싱으로 그린다.
 * @details
 *  - 슬롯 0: 기본 버텍스 버퍼(텍스트 메쉬)
 *  - 슬롯 1: 인스턴스 버퍼(FTextInstance 배열)
 *  - 그리기 전 기존 VS/InputLayout 상태를 백업하고, 인스턴싱용 VS/IL로 교체한 뒤 DrawInstanced 호출 후 복원한다.
 */
void URenderer::DrawInstanced(UMesh* text, const TArray<FTextInstance>& instances)
{
	//if (!text || instances.empty()) return;
	 
	// UPDATE CONSTABUFFER 
	UpdateTextInstanceVB(instances);
	 
	 // 이전 상태 백업하고
	 // vertexshader inputlayout을 intaced draw 용으로 교체
	ID3D11VertexShader* prevVS = nullptr; 
	ID3D11InputLayout* prevIL = nullptr;
	deviceContext->VSGetShader(&prevVS, nullptr, nullptr);
	deviceContext->IAGetInputLayout(&prevIL);

	deviceContext->VSSetShader(textVertexShaderInst, nullptr, 0);   
	deviceContext->PSSetShader(textPixelShaderInst, nullptr, 0);   
	deviceContext->IASetInputLayout(InputLayoutTextInst);

	ID3D11Buffer* bufs[2] = { text->VertexBuffer, textInstanceVB };
	UINT strides[2] = { text->Stride, (UINT)sizeof(FTextInstance) };
	UINT offsets[2] = { 0, 0 };

	deviceContext->IASetVertexBuffers(0, 2, bufs, strides, offsets);
	deviceContext->IASetPrimitiveTopology(text->PrimitiveType);

	deviceContext->DrawInstanced(text->NumVertices, (UINT)instances.size(), 0, 0); 
	
	// 이전 상태 복원 
	deviceContext->VSSetShader(prevVS, nullptr, 0);
	deviceContext->IASetInputLayout(prevIL);
	if (prevVS) prevVS->Release();
	if (prevIL) prevIL->Release(); 
}

void URenderer::BuildAabbLineVerts(const FVector& mn, const FVector& mx, TArray<FVertexPosUV4>& out)
{
	out.clear();
	out.reserve(24);

	auto V = [&](float x, float y, float z) {
		FVertexPosUV4 v{};
		v.x = x; v.y = y; v.z = z; v.w = 1.0f;
		v.r = 1.0f; v.g = 1.0f; v.b = 1.0f; v.a = 1.0f;
		v.u = 0.0f; v.v = 0.0f; // UV 기본값 설정
		out.push_back(v);
		};

	// 8 corners
	FVector c000{ mn.X,mn.Y,mn.Z }, c100{ mx.X,mn.Y,mn.Z }, 
		c110{ mx.X,mx.Y,mn.Z }, c010{ mn.X,mx.Y,mn.Z },		
		c001{ mn.X,mn.Y,mx.Z }, c101{ mx.X,mn.Y,mx.Z },		
		c111{ mx.X,mx.Y,mx.Z }, c011{ mn.X,mx.Y,mx.Z };		

	// bottom face
	V(c000.X, c000.Y, c000.Z); V(c100.X, c100.Y, c100.Z);
	V(c100.X, c100.Y, c100.Z); V(c110.X, c110.Y, c110.Z);
	V(c110.X, c110.Y, c110.Z); V(c010.X, c010.Y, c010.Z);
	V(c010.X, c010.Y, c010.Z); V(c000.X, c000.Y, c000.Z);
	// top face
	V(c001.X, c001.Y, c001.Z); V(c101.X, c101.Y, c101.Z);
	V(c101.X, c101.Y, c101.Z); V(c111.X, c111.Y, c111.Z);
	V(c111.X, c111.Y, c111.Z); V(c011.X, c011.Y, c011.Z);
	V(c011.X, c011.Y, c011.Z); V(c001.X, c001.Y, c001.Z);
	// verticals
	V(c000.X, c000.Y, c000.Z); 
	V(c001.X, c001.Y, c001.Z);

	V(c100.X, c100.Y, c100.Z); 
	V(c101.X, c101.Y, c101.Z);
	
	V(c110.X, c110.Y, c110.Z); 
	V(c111.X, c111.Y, c111.Z);
	
	V(c010.X, c010.Y, c010.Z);
	V(c011.X, c011.Y, c011.Z);
}

void URenderer::EnsureAabbLineVB(UINT bytes)
{  
	if (aabbLineVB != nullptr) return;

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = bytes;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device->CreateBuffer(&bd, nullptr, &aabbLineVB);

	if (FAILED(hr))
	{
		LogError("AABB BUFFER ERROR", hr);
		return;
	}
}

void URenderer::DrawAABBLines(const FVector& mn, const FVector& mx)
{
	//SetModel( )
	TArray<FVertexPosUV4> verts;
	BuildAabbLineVerts(mn, mx, verts);

	UINT bytes = (UINT)(verts.size() * sizeof(FVertexPosUV4));
	EnsureAabbLineVB(bytes);

	D3D11_MAPPED_SUBRESOURCE mapped{};
	deviceContext->Map(aabbLineVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, verts.data(), bytes);
	deviceContext->Unmap(aabbLineVB, 0);

	UINT stride = sizeof(FVertexPosUV4), offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &aabbLineVB, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	FMatrix identity = FMatrix::Identity;
	FVector4 color(1, 1, 0, 1); // 노란색, 필요하면 파라미터로
	SetModel(identity, color, true);
	deviceContext->Draw((UINT)verts.size(), 0);
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

void URenderer::SetRasterizerMode(bool isSolid)
{
	auto& rss = isSolid ? rasterizerState_solid : rasterizerState_wireframe;
	//if (!rss)
	//	return;
	deviceContext->RSSetState(rss);
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

void URenderer::SetShader(UShader* vertexShader, UShader* pixelShader)
{
	if (bIsShaderReflectionEnabled)
	{
		currentVertexShader = vertexShader ? vertexShader : VertexShader_SR.get();
		currentPixelShader = pixelShader ? pixelShader : PixelShader_SR.get();
	}
}

void URenderer::SetModel(const FMatrix& M, const FVector4& color, bool bIsSelected)
{
	// per-object: MVP = M * VP
	FMatrix MVP = M * mVP;
	if (!bIsShaderReflectionEnabled)
	{
		CopyRowMajor(mCBData.MVP, MVP);
		memcpy(mCBData.MeshColor, &color, sizeof(float) * 4);
		mCBData.IsSelected = bIsSelected ? 1.0f : 0.0f;
		UpdateConstantBuffer(&mCBData, sizeof(mCBData));
	}
	else
	{
		(*currentVertexShader)["ConstantBuffer"]["MVP"] = MVP;
		(*currentVertexShader)["ConstantBuffer"]["MeshColor"] = color;

		/** @brief: For now, binding should be done here. */
		currentVertexShader->Bind(GetDeviceContext(), "ConstantBuffer");
		currentPixelShader->Bind(GetDeviceContext());
	}
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
  
void URenderer::SetSampler(ID3D11SamplerState* sampler, UINT slot)
{
	if (deviceContext && sampler)
		deviceContext->PSSetSamplers(slot, 1, &sampler);
} 

void URenderer::DumpVSInputSignature(ID3DBlob* vsBlob)
{
	ID3D11ShaderReflection* refl;
	D3DReflect(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&refl);

	D3D11_SHADER_DESC sd = {};
	refl->GetDesc(&sd);

	OutputDebugStringA("== VS INPUT PARAMETERS ==\n");
	for (UINT i = 0; i < sd.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC spd{};
		refl->GetInputParameterDesc(i, &spd);

		char buf[256];
		sprintf_s(buf, "  %s%d  mask=0x%x  reg=%u\n",
			spd.SemanticName, spd.SemanticIndex, spd.Mask, spd.Register);
		OutputDebugStringA(buf);
	}
	OutputDebugStringA("=========================\n");
}