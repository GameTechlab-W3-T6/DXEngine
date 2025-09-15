#include "stdafx.h"

#include "UBatchRenderer.h"
#include "UClass.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"

IMPLEMENT_UCLASS(UBatchRenderer, URenderer)

void UBatchRenderer::DrawPrimitiveComponent(UPrimitiveComponent* Component)
{
	/** @todo: Each component has own layer ID later on. For now, use hard-coded version. */
	uint32 Layer = Component->GetLayer();
	auto MeshID = Component->GetMesh()->GetID();
	auto VertexShaderID = Component->GetVertexShader()->GetID();
	auto PixelShaderID = Component->GetPixelShader()->GetID();

	RenderKeyType RenderKey = RenderKeyManager::CreateKey(
		*MeshID,
		VertexShaderID,
		PixelShaderID,
		Layer
	);

	//SceneComponentMap.emplace(RenderKey, Component);
	PrimitiveComponentArray.emplace_back(RenderKey, Component);
	//URenderer::DrawPrimitiveComponent(Component);
}

[[deprecated]] void UBatchRenderer::DrawGizmoComponent(UGizmoComponent* Component, bool drawOnTop) 
{
	/** @todo: Each component has own layer ID later on. For now, use hard-coded version. */
	uint32 Layer = 0;
	auto MeshID = Component->GetMesh()->GetID();
	auto VertexShaderID = Component->GetVertexShader()->GetID();
	auto PixelShaderID = Component->GetPixelShader()->GetID();

	RenderKeyType RenderKey = RenderKeyManager::CreateKey(
		*MeshID,
		VertexShaderID,
		PixelShaderID,
		Layer
	);

	//SceneComponentMap.emplace(RenderKey, Component);
	PrimitiveComponentArray.emplace_back(RenderKey, Component);
	//URenderer::DrawGizmoComponent(Component, drawOnTop);
}

void UBatchRenderer::Draw()
{
	if (PrimitiveComponentArray.empty())
	{
		return;
	}

	std::sort(PrimitiveComponentArray.begin(), PrimitiveComponentArray.end(),
		[](const auto& lhs, const auto& rhs) {
		return lhs.first > rhs.first;
	});

	uint32 LastLayer = -1;
	uint32 LastMeshID = -1;
	uint32 LastVertexShaderID = -1;
	uint32 LastPixelShaderID = -1;

	UE_LOG("DRAW START ===============");

	for (auto Iter = PrimitiveComponentArray.begin(); Iter != PrimitiveComponentArray.end(); ++Iter)
	{
		auto [RenderKey, Component] = *Iter;

		Component->UpdateConstantBuffer(*this);

		auto Layer = RenderKeyManager::Get<LayerField>(RenderKey);
		auto MeshID = RenderKeyManager::Get<MeshField>(RenderKey);
		auto VertexShaderID = RenderKeyManager::Get<VertexShaderField>(RenderKey);
		auto PixelShaderID = RenderKeyManager::Get<PixelShaderField>(RenderKey);

		if (Iter == PrimitiveComponentArray.begin())
		//if (true)
		{
			Component->BindVertexShader(*this);

			Component->BindPixelShader(*this);

			Component->BindMesh(*this);
		}
		else
		{
			UE_LOG("Last Layer: %d", LastLayer);
			UE_LOG("Layer: %d", Layer);
			if (Layer != LastLayer)
			{
				UE_LOG("CLEAR!");
				GetDeviceContext()->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			}

			if (VertexShaderID != LastVertexShaderID)
			{
				Component->BindVertexShader(*this);
			}

			//if (PixelShaderID != LastPixelShaderID)
			if (true)
			{
				Component->BindPixelShader(*this);
			}
			
			if (MeshID != LastMeshID)
			{
				Component->BindMesh(*this);
			}
		}

		auto Mesh = Component->GetMesh();
		if (Mesh->IsIndexBufferEnabled())
		{
			DeviceContext->DrawIndexed(Mesh->NumIndices, 0, 0);
		}
		else
		{
			DeviceContext->Draw(Mesh->NumVertices, 0);
		}


		LastLayer = Layer;
		LastMeshID = MeshID;
		LastVertexShaderID = VertexShaderID;
		LastPixelShaderID = PixelShaderID;
	}

	UE_LOG("DRAW END ===============");

	PrimitiveComponentArray.clear();
}
