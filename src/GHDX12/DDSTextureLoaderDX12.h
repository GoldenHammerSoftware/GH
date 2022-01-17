#pragma once

#include "GHPlatform/win32/GHWin32Include.h"
#include <memory>
#include <vector>
#include "GHDX12Include.h"

// Heavily based on https://github.com/microsoft/DirectXTK12/wiki/DDSTextureLoader
namespace DDSTextureLoaderDX12
{
    HRESULT LoadDDSTextureFromFile(
        ID3D12Device* d3dDevice,
        const wchar_t* szFileName,
        ID3D12Resource** texture,
        std::unique_ptr<uint8_t[]>& ddsData,
        std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        size_t maxsize = 0,
        bool* isCubeMap = nullptr);
};