#pragma once

struct FTexture
{	
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11SamplerState* samplerState = nullptr;

	void Release();
};
