#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Shader.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UTextholderComp.h"
#include "UEngineSubsystem.h"
#include "ViewModeIndex.h"

class UPrimitiveComponent;

// URenderer.h or cpp 상단
struct CBTransform
{
	// HLSL의 row_major float4x4와 메모리 호환을 위해 float[16]로 보냄
	float MVP[16];
	float MeshColor[4];
	float IsSelected;
	float padding[3];
};

class URenderer : public UEngineSubsystem
{
	DECLARE_UCLASS(URenderer, UEngineSubsystem)
public:
	virtual ~URenderer();

	URenderer();

	// ========================================================================== 
	// Renderer Core (Resource Managment)

	/** @todo: Refactor resource management by using ComPtr. */
public:
	/** Initialization and cleanup */
	bool Initialize(HWND hWnd);
	bool CreateShader();
	bool CreateShader_SR();
	bool CreateRasterizerState();
	bool CreateConstantBuffer();
	bool CreateInstancedVB();
	void Release();
	void ReleaseShader();
	void ReleaseConstantBuffer();

	/** Buffer */
	[[deprecated]] ID3D11Buffer* CreateVertexBuffer(const void* Data, size_t Size);
	[[deprecated]] ID3D11Buffer* CreateIndexBuffer(const void* Data, size_t Size);
	[[deprecated]] bool ReleaseVertexBuffer(ID3D11Buffer* Buffer);
	[[deprecated]] bool ReleaseIndexBuffer(ID3D11Buffer* Buffer);

	/** Texture */
	ID3D11Texture2D* CreateTexture2D(int32 Width, int32 Height, DXGI_FORMAT Format, const void* Data = nullptr);
	ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* Texture);
	bool ReleaseTexture(ID3D11Texture2D* Texture);
	bool ReleaseShaderResourceView(ID3D11ShaderResourceView* ShaderResourceView);

	// Getters
	ID3D11Device* GetDevice() const { return Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }
	IDXGISwapChain* GetSwapChain() const { return SwapChain; }
	bool IsInitialized() const { return bIsInitialized; }

protected:
	/** Renderer State */
	bool bIsInitialized;

	/** Core D3D11 Objects */
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	IDXGISwapChain* SwapChain;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RasterizerState* RasterizerStateSolid;
	ID3D11RasterizerState* RasterizerStateWireFrame;

	/** Shader Objects */
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;

	/** Shader Objects using Shader Reflection */
	TUniquePtr<UShader> VertexShader_SR;
	TUniquePtr<UShader> PixelShader_SR;

	UShader* currentVertexShader;
	UShader* currentPixelShader;

	ID3D11Buffer* ConstantBuffer;

	// =================================================== //
	// Text
	ID3D11VertexShader* textVertexShaderInst;
	ID3D11PixelShader* textPixelShaderInst;
	ID3D11InputLayout* InputLayoutTextInst;
	ID3D11Buffer* textInstanceVB; 
	// =================================================== //


	/** Window handle */
	HWND hWnd;

	// ========================================================================== 
	// Rendering Features

public:
	/** Rendering operations */
	void Prepare();
	void PrepareShader();
	virtual void SwapBuffer();
	void Clear(float Red = 0.0f, float Green = 0.0f, float Blue = 0.0f, float Alpha = 1.0f);

	// Drawing operations
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation = 0, INT BaseVertexLocation = 0);
	/** @note: Use Draw() or DrawMesh() to track number of draw calls */
	void Draw(UINT VertexCount, UINT StartVertexLocation = 0);
	void DrawMesh(UMesh* Mesh);

	/** @note: Does nothing in URenderer.h. Just introduced for derived classes. */
	virtual void Draw() {}

	/** @todo */
	//virtual void DrawPrimitive(UPrimitiveComponent* PrimitiveComponent)
	//{
	//	return;
	//}

	virtual void DrawPrimitiveComponent(UPrimitiveComponent* component);
	virtual void DrawGizmoComponent(UGizmoComponent* component, bool drawOnTop = false);
	virtual void DrawTextholderComponent(UTextholderComp* Component);

	/** @note: These helper functions use Draw() or DrawMesh() Internally. */
	void DrawLine(UMesh* Mesh);
	void DrawMeshOnTop(UMesh* Mesh);

	/** @note: These helper functions use Draw AABB */
	ID3D11Buffer* aabbLineVB;

	void BuildAabbLineVerts(const FVector& mn, const FVector& mx, TArray<FVertexPosColorUV4>& out);
	void EnsureAabbLineVB(UINT bytes);
	void DrawAABBLines(const FVector& mn, const FVector& mx);


	/** Resource binding */
	[[deprecated]] void SetVertexBuffer(ID3D11Buffer* Buffer, UINT Stride, UINT Offset = 0);
	[[deprecated]] void SetIndexBuffer(ID3D11Buffer* Buffer, DXGI_FORMAT Format = DXGI_FORMAT_R32_UINT);
	[[deprecated]] void SetConstantBuffer(ID3D11Buffer* Buffer, UINT Slot = 0);
	void SetShader(UShader* vertexShader, UShader* pixelShader);
	void SetTexture(ID3D11ShaderResourceView* ShaderResourceView, UINT Slot = 0);
	void SetRasterizerMode(EViewModeIndex vmi);

	/** Constant buffer updates */
	void SetViewProj(const FMatrix& View, const FMatrix& Projection); // 내부에 VP 캐시
	void SetModel(const FMatrix& Model, const FVector4& Color, bool IsSelected);
	bool UpdateConstantBuffer(const void* data, size_t sizeInBytes);

	FMatrix GetViewProj() const
	{
		return VP;
	}

	/** Window resize handling */
	bool ResizeBuffers(int32 width, int32 height);

	/** Viewport */
	void SetTargetAspect(float Aspect)
	{
		if (Aspect > 0.0f)
		{
			TargetAspect = Aspect;
		}
	}

	// targetAspect를 내부에서 사용 (카메라에 의존 X)
	D3D11_VIEWPORT MakeAspectFitViewport(int32 Width, int32 Height) const;

	// 드래그 중 호출: currentViewport만 갈아끼움
	void UseAspectFitViewport(int32 Width, int32 Height)
	{
		CurrentViewport = MakeAspectFitViewport(Width, Height);
	}

	// 평소엔 풀 윈도우
	void UseFullWindowViewport()
	{
		CurrentViewport = Viewport;
	}

private:
	/** Internal helper functions */
	bool CreateDeviceAndSwapChain(HWND windowHandle);
	bool CreateRenderTargetView();
	bool CreateDepthStencilView(int32 width, int32 height);
	bool SetupViewport(int32 width, int32 height);

private:
	/** Viewport */
	D3D11_VIEWPORT Viewport;
	D3D11_VIEWPORT CurrentViewport; // 실제로 사용할 뷰포트(레터박스/필러박스 포함)
	float TargetAspect = 16.0f / 9.0f;

	FMatrix VP;                 // 프레임 캐시
	CBTransform MCBData;

	// ========================================================================== //
	// Utility Features
public:
	bool CheckDeviceState();
	void GetBackBufferSize(int32& Width, int32& Height);
	uint64 GetDrawCallCount() const
	{
		return DrawCallCount;
	}
	uint64 GetMeshSwitchCount() const
	{
		return MeshSwitchCount;
	}
	uint64 GetVertexShaderSwitchCount() const
	{
		return VertexShaderSwitchCount;
	}
	uint64 GetPixelShaderSwitchCount()
	{
		return PixelShaderSwitchCount;
	}
	uint64 GetDepthStencilViewClearCount()
	{
		return DepthStencilViewClearCount;
	}

protected:
	void IncrementDrawCallCount() { ++DrawCallCount; }
	void IncrementMeshSwitchCount() { ++MeshSwitchCount; }
	void IncrementVertexShaderSwitchCount() { ++VertexShaderSwitchCount; }
	void IncrementPixelShaderSwitchCount() { ++PixelShaderSwitchCount; }
	void IncrementDepthStencilViewClearCount() { ++DepthStencilViewClearCount; }

private:
	/** Error handling */
	void LogError(HRESULT hResult, const char* Function);
	bool CheckResult(HRESULT hResult, const char* Function);

	/** @brief: Helper function for copying matrix. */
	static inline void CopyRowMajor(float Destination[16], const FMatrix& Source)
	{
		for (int32 Row = 0; Row < 4; ++Row) {
			for (int32 Column = 0; Column < 4; ++Column) {
				Destination[Row * 4 + Column] = Source.M[Row][Column];
			}
		}
	}

private:
	bool bIsShaderReflectionEnabled;

	uint64 DrawCallCount;
	/** @brief: The number of VBO binding. */
	uint64 MeshSwitchCount;
	/** @brief: The number of Veretx Shader binding. */
	uint64 VertexShaderSwitchCount;
	/** @brief: The number of Pixel Shader binding. */
	uint64 PixelShaderSwitchCount;
	/** @brief: The number of Depth Stencil View clearing for layers. */
	uint64 DepthStencilViewClearCount;
};