#include "stdafx.h"

#include "UBatchRenderer.h"
#include "UClass.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "UTextholderComp.h"
#include "UGizmoComponent.h"

IMPLEMENT_UCLASS(UBatchRenderer, URenderer)

void UBatchRenderer::DrawPrimitiveComponent(UPrimitiveComponent* Component)
{
	assert(Component && "Component is not valid.");

	LayerID Layer = Component->GetLayer();
	MeshID Mesh = Component->GetMesh()->GetID();
	ShaderID VertexShader = Component->GetVertexShader()->GetID();
	ShaderID PixelShader = Component->GetPixelShader()->GetID();

	RenderKeyType RenderKey = RenderKeyManager::CreateKey(Mesh, PixelShader, VertexShader, Layer);

	PrimitiveComponentArray.emplace_back(RenderKey, Component);
}

/** @note: drawOnTop does nothing with this function. */
/** @todo: Resolve naming collision with URenderer::Draw. */
void UBatchRenderer::DrawGizmoComponent(UGizmoComponent* Component, bool drawOnTop) 
{
	DrawPrimitiveComponent(Component);
}

/** @todo: Temporary implmentation which draws text holder at last by additional array. */
void UBatchRenderer::DrawTextholderComponent(UTextholderComp* Component)
{
    TextholderComponentArray.push_back(Component);
}

void UBatchRenderer::Draw()
{
	if (PrimitiveComponentArray.empty())
	{
		return;
	}

	/** @todo: Is std::sort enough? */
	std::sort(PrimitiveComponentArray.begin(), PrimitiveComponentArray.end(),
		[](const auto& lhs, const auto& rhs) {
		return lhs.first > rhs.first;
	});

	/** @note: Be careful not to use uninitialized values. */
	TOptional<LayerID> LastLayer;
	TOptional<MeshID> LastMesh;
	TOptional<ShaderID> LastVertexShader;
	TOptional<ShaderID> LastPixelShader;

	for (const auto& [RenderKey, Component] : PrimitiveComponentArray)
	{
		const LayerID Layer = RenderKeyManager::Get<LayerField>(RenderKey);
		const MeshID Mesh = RenderKeyManager::Get<MeshField>(RenderKey);
		const ShaderID VertexShader = RenderKeyManager::Get<VertexShaderField>(RenderKey);
		const ShaderID PixelShader = RenderKeyManager::Get<PixelShaderField>(RenderKey);

		Component->UpdateConstantBuffer(*this);

		/** If first element arrives, state should be initialized. */
		if (Layer != LastLayer)
		{
			GetDeviceContext()->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			LastLayer = Layer;
			IncrementDepthStencilViewClearCount();
		}

		if (VertexShader != LastVertexShader)
		{
			Component->BindVertexShader(*this);
			LastVertexShader = VertexShader;
			IncrementVertexShaderSwitchCount();
		}

		if (PixelShader != LastPixelShader)
		{
			Component->BindPixelShader(*this);
			LastPixelShader = PixelShader;
			IncrementPixelShaderSwitchCount();
		}

		if (Mesh != LastMesh)
		{
			Component->BindMesh(*this);
			LastMesh = Mesh;
			IncrementMeshSwitchCount();
		}

		auto pMesh = Component->GetMesh();
		if (pMesh->IsIndexBufferEnabled())
		{
			DrawIndexed(pMesh->NumIndices, 0, 0);
		}
		else
		{
			URenderer::Draw(pMesh->NumVertices, 0);
		}
	}

    GetDeviceContext()->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (auto Component : TextholderComponentArray)
    {
        URenderer::DrawTextholderComponent(Component);
    }
	PrimitiveComponentArray.clear();
    TextholderComponentArray.clear();
}
