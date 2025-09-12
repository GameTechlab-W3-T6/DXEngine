#include "stdafx.h"
#include "FTexture.h"
 
 

void FTexture::Release()
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

