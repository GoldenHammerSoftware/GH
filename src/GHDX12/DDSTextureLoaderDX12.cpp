#include "DDSTextureLoaderDX12.h"

namespace DDSTextureLoaderDX12
{
    HRESULT LoadDDSTextureFromFile(
        ID3D12Device* d3dDevice,
        const wchar_t* szFileName,
        ID3D12Resource** texture,
        std::unique_ptr<uint8_t[]>& ddsData,
        std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        size_t maxsize,
        bool* isCubeMap)
    {
        return S_OK;
    }

};