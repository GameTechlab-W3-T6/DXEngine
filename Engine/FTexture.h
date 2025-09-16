#pragma once

#include <d3d11.h>

struct FTexture
{
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11SamplerState* samplerState = nullptr;

	int width;
	int height;

	void Bind(ID3D11DeviceContext* DeviceContext, UINT Slot)
	{
		assert(DeviceContext && srv && samplerState);

		DeviceContext->PSSetShaderResources(Slot, 1, &srv);

		DeviceContext->PSSetSamplers(Slot, 1, &samplerState);
	}

	void Release()
	{
		if (samplerState)
		{
			samplerState->Release();
			samplerState = nullptr;
		}

		if (srv)
		{
			srv->Release();
			srv = nullptr;
		}
	}
};
