//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.cpp
//
// Function for loading a DDS texture and creating a Direct3D 11 runtime resource for it
//
// Note these functions are useful as a light-weight runtime loader for DDS files. For
// a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#include <dxgiformat.h>
#include <assert.h>
#include <algorithm>
#include <memory>
#include <assert.h>

#include "DDSTextureLoader.h"
#include "Render/GHDXGIUtil.h"
#include "Render/GHDDSUtil.h"

namespace GHDDSLoader
{


//--------------------------------------------------------------------------------------
static HRESULT CreateD3DResources( _In_ ID3D11Device* d3dDevice,
                                   _In_ uint32_t resDim,
                                   _In_ size_t width,
                                   _In_ size_t height,
                                   _In_ size_t depth,
                                   _In_ size_t mipCount,
                                   _In_ size_t arraySize,
                                   _In_ DXGI_FORMAT format,
                                   _In_ bool isCubeMap,
                                   _In_count_(mipCount*arraySize) D3D11_SUBRESOURCE_DATA* initData,
                                   _Out_opt_ ID3D11Resource** texture,
                                   _Out_opt_ ID3D11ShaderResourceView** textureView )
{
    if ( !d3dDevice || !initData )
        return E_POINTER;

    HRESULT hr = E_FAIL;

    switch ( resDim ) 
    {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            {
                D3D11_TEXTURE1D_DESC desc;
                desc.Width = static_cast<UINT>( width ); 
                desc.MipLevels = static_cast<UINT>( mipCount );
                desc.ArraySize = static_cast<UINT>( arraySize );
                desc.Format = format;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;

                ID3D11Texture1D* tex = nullptr;
                hr = d3dDevice->CreateTexture1D( &desc,
                                                 initData,
                                                 &tex
                                               );
                if (SUCCEEDED( hr ) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset( &SRVDesc, 0, sizeof( SRVDesc ) );
                        SRVDesc.Format = format;

                        if (arraySize > 1)
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
                            SRVDesc.Texture1DArray.MipLevels = desc.MipLevels;
                            SRVDesc.Texture1DArray.ArraySize = static_cast<UINT>( arraySize );
                        }
                        else
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                            SRVDesc.Texture1D.MipLevels = desc.MipLevels;
                        }

                        hr = d3dDevice->CreateShaderResourceView( tex,
                                                                  &SRVDesc,
                                                                  textureView
                                                                );
                        if ( FAILED(hr) )
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
/*
#if defined(_DEBUG) || defined(PROFILE)
                        tex->SetPrivateData( WKPDID_D3DDebugObjectName,
                                             sizeof("DDSTextureLoader")-1,
                                             "DDSTextureLoader" );
#endif
*/
                        tex->Release();
                    }
                }
            }
           break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                D3D11_TEXTURE2D_DESC desc;
                desc.Width = static_cast<UINT>( width );
                desc.Height = static_cast<UINT>( height );
                desc.MipLevels = static_cast<UINT>( mipCount );
                desc.ArraySize = static_cast<UINT>( arraySize );
                desc.Format = format;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = (isCubeMap) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

                ID3D11Texture2D* tex = nullptr;
                hr = d3dDevice->CreateTexture2D( &desc,
                                                 initData,
                                                 &tex
                                               );
                if (SUCCEEDED( hr ) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset( &SRVDesc, 0, sizeof( SRVDesc ) );
                        SRVDesc.Format = format;

                        if (isCubeMap)
                        {
                            if (arraySize > 6)
                            {
                                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                                SRVDesc.TextureCubeArray.MipLevels = desc.MipLevels;

                                // Earlier we set arraySize to (NumCubes * 6)
                                SRVDesc.TextureCubeArray.NumCubes = static_cast<UINT>( arraySize / 6 );
                            }
                            else
                            {
                                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                                SRVDesc.TextureCube.MipLevels = desc.MipLevels;
                            }
                        }
                        else if (arraySize > 1)
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                            SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
                            SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>( arraySize );
                        }
                        else
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                            SRVDesc.Texture2D.MipLevels = desc.MipLevels;
                        }

                        hr = d3dDevice->CreateShaderResourceView( tex,
                                                                  &SRVDesc,
                                                                  textureView
                                                                );
                        if ( FAILED(hr) )
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
/*
#if defined(_DEBUG) || defined(PROFILE)
                        tex->SetPrivateData( WKPDID_D3DDebugObjectName,
                                             sizeof("DDSTextureLoader")-1,
                                             "DDSTextureLoader"
                                           );
#endif
*/
                        tex->Release();
                    }
                }
            }
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            {
                D3D11_TEXTURE3D_DESC desc;
                desc.Width = static_cast<UINT>( width );
                desc.Height = static_cast<UINT>( height );
                desc.Depth = static_cast<UINT>( depth );
                desc.MipLevels = static_cast<UINT>( mipCount );
                desc.Format = format;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;

                ID3D11Texture3D* tex = nullptr;
                hr = d3dDevice->CreateTexture3D( &desc,
                                                 initData,
                                                 &tex
                                               );
                if (SUCCEEDED( hr ) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset( &SRVDesc, 0, sizeof( SRVDesc ) );
                        SRVDesc.Format = format;
                        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                        SRVDesc.Texture3D.MipLevels = desc.MipLevels;

                        hr = d3dDevice->CreateShaderResourceView( tex,
                                                                  &SRVDesc,
                                                                  textureView
                                                                );
                        if ( FAILED(hr) )
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
/*
#if defined(_DEBUG) || defined(PROFILE)
                        tex->SetPrivateData( WKPDID_D3DDebugObjectName,
                                             sizeof("DDSTextureLoader")-1,
                                             "DDSTextureLoader" );
#endif
*/
                        tex->Release();
                    }
                }
            }
            break; 
    }

    return hr;
}

//--------------------------------------------------------------------------------------
static HRESULT CreateTextureFromDDS(_In_ ID3D11Device* d3dDevice,
    _In_ const GHDDSUtil::DDS_HEADER* header,
    _In_bytecount_(bitSize) const uint8_t* bitData,
    _In_ size_t bitSize,
    const GHDDSUtil::DDSDesc& desc,
    GHDDSUtil::SubresourceData* initData,
    _Out_opt_ ID3D11Resource** texture,
    _Out_opt_ ID3D11ShaderResourceView** textureView,
    _In_ size_t maxsize)
{
    HRESULT hr = S_OK;

    static_assert(sizeof(GHDDSUtil::SubresourceData) == sizeof(D3D11_SUBRESOURCE_DATA), "SubresourceData must match D3D11_SUBRESOURCE_DATA");

    bool ret = CreateD3DResources( d3dDevice, desc.resDim, desc.twidth, desc.theight, desc.tdepth, desc.mipCount - desc.skipMip, desc.arraySize, (DXGI_FORMAT)desc.format, desc.isCubeMap, (D3D11_SUBRESOURCE_DATA*)initData, texture, textureView );

    return ret ? S_OK : E_FAIL;
}

struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };

typedef public std::unique_ptr<void, handle_closer> ScopedHandle;

inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

//--------------------------------------------------------------------------------------
HRESULT LoadTextureDataFromFile(_In_z_ const wchar_t* fileName,
    std::unique_ptr<uint8_t[]>& ddsData,
    GHDDSUtil::DDS_HEADER** header,
    uint8_t** bitData,
    size_t* bitSize
)
{
    if (!header || !bitData || !bitSize)
    {
        return E_POINTER;
    }

    // open the file
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    ScopedHandle hFile(safe_handle(CreateFile2(fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        OPEN_EXISTING,
        nullptr)));
#else
    ScopedHandle hFile(safe_handle(CreateFileW(fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr)));
#endif

    if (!hFile)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Get the file size
    LARGE_INTEGER FileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
    FILE_STANDARD_INFO fileInfo;
    if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    FileSize = fileInfo.EndOfFile;
#else
    GetFileSizeEx(hFile.get(), &FileSize);
#endif

    // File is too big for 32-bit allocation, so reject read
    if (FileSize.HighPart > 0)
    {
        return E_FAIL;
    }

    // Need at least enough data to fill the header and magic number to be a valid DDS
    if (FileSize.LowPart < (sizeof(GHDDSUtil::DDS_HEADER) + sizeof(uint32_t)))
    {
        return E_FAIL;
    }

    // create enough space for the file data
    ddsData.reset(new uint8_t[FileSize.LowPart]);
    if (!ddsData)
    {
        return E_OUTOFMEMORY;
    }

    // read the data in
    DWORD BytesRead = 0;
    if (!ReadFile(hFile.get(),
        ddsData.get(),
        FileSize.LowPart,
        &BytesRead,
        nullptr
    ))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (BytesRead < FileSize.LowPart)
    {
        return E_FAIL;
    }

    bool ret = GHDDSUtil::LoadTextureDataFromMemory(ddsData.get(), FileSize.LowPart, header, bitData, bitSize);
    if (ret) return S_OK;
    return E_FAIL;
}

//--------------------------------------------------------------------------------------
HRESULT CreateDDSTextureFromFile( _In_ ID3D11Device* d3dDevice,
                                  _In_z_ const wchar_t* fileName,
                                  _Out_opt_ ID3D11Resource** texture,
                                  _Out_opt_ ID3D11ShaderResourceView** textureView,
                                  _In_ size_t maxsize )
{
    if (!d3dDevice || !fileName || (!texture && !textureView))
    {
        return E_INVALIDARG;
    }

    GHDDSUtil::DDS_HEADER* header = nullptr;
    uint8_t* bitData = nullptr;
    size_t bitSize = 0;

    std::unique_ptr<uint8_t[]> ddsData;
    HRESULT hr = LoadTextureDataFromFile( fileName,
                                          ddsData,
                                          &header,
                                          &bitData,
                                          &bitSize
                                        );
    if (FAILED(hr))
    {
        return hr;
    }

    GHDDSUtil::DDSDesc desc;
    hr = GHDDSUtil::validateAndParseHeader(*header, desc);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create the texture data
    std::unique_ptr<GHDDSUtil::SubresourceData> initData(new GHDDSUtil::SubresourceData[desc.mipCount * desc.arraySize]);
    if (!initData)
    {
        return E_OUTOFMEMORY;
    }

    hr = GHDDSUtil::FillInitData(desc, maxsize, bitSize, bitData, initData.get());
    if (FAILED(hr))
    {
        return hr;
    }

    hr = CreateTextureFromDDS(d3dDevice,
        header,
        bitData,
        bitSize,
        desc,
        initData.get(),
        texture,
        textureView,
        maxsize
    );
    
    if (FAILED(hr) && !maxsize && (desc.mipCount > 1))
    {
        // Retry with a maxsize determined by feature level
        switch (d3dDevice->GetFeatureLevel())
        {
        case D3D_FEATURE_LEVEL_9_1:
        case D3D_FEATURE_LEVEL_9_2:
            if (desc.isCubeMap)
            {
                maxsize = 512 /*D3D_FL9_1_REQ_TEXTURECUBE_DIMENSION*/;
            }
            else
            {
                maxsize = (desc.resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                    ? 256 /*D3D_FL9_1_REQ_TEXTURE3D_U_V_OR_W_DIMENSION*/
                    : 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
            }
            break;

        case D3D_FEATURE_LEVEL_9_3:
            maxsize = (desc.resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                ? 256 /*D3D_FL9_1_REQ_TEXTURE3D_U_V_OR_W_DIMENSION*/
                : 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
            break;

        default: // D3D_FEATURE_LEVEL_10_0 & D3D_FEATURE_LEVEL_10_1
            maxsize = (desc.resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                ? 2048 /*D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION*/
                : 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
            break;
        }

        static_assert(sizeof(GHDDSUtil::SubresourceData) == sizeof(D3D11_SUBRESOURCE_DATA), "SubresourceData must match D3D11_SUBRESOURCE_DATA");
        hr = FillInitData(desc, maxsize, bitSize, bitData, (GHDDSUtil::SubresourceData*)(initData.get()));
        if (SUCCEEDED(hr))
        {
            hr = CreateTextureFromDDS(d3dDevice,
                header,
                bitData,
                bitSize,
                desc,
                initData.get(),
                texture,
                textureView,
                maxsize
            );
        }
    }

    /*
#if defined(_DEBUG) || defined(PROFILE)
    if (texture != 0 || textureView != 0)
    {
        CHAR strFileA[MAX_PATH];
        WideCharToMultiByte( CP_ACP,
                             WC_NO_BEST_FIT_CHARS,
                             fileName,
                             -1,
                             strFileA,
                             MAX_PATH,
                             nullptr,
                             FALSE
                           );
        const CHAR* pstrName = strrchr( strFileA, '\\' );
        if (!pstrName)
        {
            pstrName = strFileA;
        }
        else
        {
            pstrName++;
        }

        if (texture != 0 && *texture != 0)
        {
            (*texture)->SetPrivateData( WKPDID_D3DDebugObjectName,
                                        static_cast<UINT>( strnlen_s(pstrName, MAX_PATH) ),
                                        pstrName
                                      );
        }

        if (textureView != 0 && *textureView != 0 )
        {
            (*textureView)->SetPrivateData( WKPDID_D3DDebugObjectName,
                                            static_cast<UINT>( strnlen_s(pstrName, MAX_PATH) ),
                                            pstrName
                                          );
        }
    }
#endif
*/
    return hr;
}

} // namespace GHDDSLoader
