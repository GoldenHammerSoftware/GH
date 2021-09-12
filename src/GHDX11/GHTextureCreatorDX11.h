// Copyright Golden Hammer Software
#pragma once

#include "GHDX11Include.h"

// interface for creating dx11 textures given a filename
class GHTextureCreatorDX11
{
public:
	virtual ~GHTextureCreatorDX11(void) {}
	
	// Reload a texture file from disk and put it into d3d structures.
	virtual bool createTexture(const char* filename,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
		void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
		bool useMipmaps,
		DXGI_FORMAT& dxFormat
		) = 0;

	// Reload a texture from memory.
	virtual bool createTextureFromMemory(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
		void* pixels, unsigned int width, unsigned int height, unsigned int depth,
		bool useMipmaps,
		DXGI_FORMAT dxFormat, bool okToDeleteImageBuf) = 0;
};
