// Copyright Golden Hammer Software
#include "GHRenderDeviceDX11Native.h"
#include "GHUtils/GHProfiler.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX11SwapChainCreator.h"
#include "GHScreenInfo.h"
#include "DXGI.h"
#include "DDSTextureLoader.h"
#include "GHTextureDX11.h"
#include "GHWin32/GHDXGIUtil.h"

GHRenderDeviceDX11Native::GHRenderDeviceDX11Native(const int& graphicsQuality,
												   GHDX11SwapChainCreator& swapChainCreator,
												   const GHScreenInfo& screenInfo,
												   GHMessageHandler& eventMgr,
												   unsigned int desiredMSAA)
: GHRenderDeviceDX11(graphicsQuality, eventMgr)
, mSwapChainCreator(swapChainCreator)
, mScreenInfo(screenInfo)
, mDesiredMSAA(desiredMSAA)
{
	createDeviceResources();
	createWindowSizeDependentResources();
}

void GHRenderDeviceDX11Native::handleGraphicsQualityChange(void)
{
	// if a graphics quality change caused a render target size change, 
	//  we need to update the swap chain.  
	// if we do this every time the window changes sizes,
	//  some resizes don't work correctly.

	int oldWidth = (int)m_renderTargetSize[0];
	int oldHeight = (int)m_renderTargetSize[1];

	GHRenderDeviceDX11::handleGraphicsQualityChange();

	if ((int)m_renderTargetSize[0] != oldWidth ||
		(int)m_renderTargetSize[1] != oldHeight)
	{
		mSwapChainCreator.swapChainUpdated(mSwapChain);
	}
}

void GHRenderDeviceDX11Native::endFrame(void)
{
	GHRenderDeviceDX11::endFrame();

	GHPROFILEBEGIN("GHRenderDeviceDX11Native::endFrame")
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	int vsync = (mVSyncEnabled ? 1 : 0);
	HRESULT hr = mSwapChain->Present(vsync, 0);
	if (hr != S_OK) {
		GHDebugMessage::outputString("Failed present");
	}
	GHPROFILEEND("GHRenderDeviceDX11Native::endFrame")
}

void GHRenderDeviceDX11Native::createDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering than the API default.
    // It is recommended usage, and is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    // If the project is in a debug build, enable debugging via SDK Layers with this flag.
    //creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

	// figure out which adapter to use
	IDXGIAdapter* adapter = nullptr;
	IDXGIFactory* dxgiFactory = nullptr;
	HRESULT factoryResult = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&dxgiFactory));
	if (FAILED(factoryResult))
	{
		GHDebugMessage::outputString("Failed to CreateDXGIFactory1");
	}
	else
	{
		// if there are more than one adapters and the default one contains "Intel" and another does not, pick a different one.
		// this helps laptops with second video cards.
		for (UINT iAdapter = 0; dxgiFactory->EnumAdapters(iAdapter, &adapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
		{
			DXGI_ADAPTER_DESC adapterDesc;
			adapter->GetDesc(&adapterDesc);
			const wchar_t* intelIndex = wcsstr(adapterDesc.Description, L"Intel");
			// microsoft basic render driver is also something we never want to use.
			const wchar_t* microsoftIndex = wcsstr(adapterDesc.Description, L"Microsoft");
			if (intelIndex || microsoftIndex)
			{
				adapter->Release();
				adapter = 0;
			}
			else
			{
				break;
			}
		}
		dxgiFactory->Release();
	}

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	if (adapter) {
		driverType = D3D_DRIVER_TYPE_UNKNOWN;
	}

    // Create the DX11 API device object, and get a corresponding context.
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	HRESULT hr = 
        D3D11CreateDevice(
            adapter,                    // specify null to use the default adapter
            driverType,
            nullptr,                    // leave as nullptr unless software device
            creationFlags,              // optionally set debug and Direct2D compatibility flags
            featureLevels,              // list of feature levels this app can support
            ARRAYSIZE(featureLevels),   // number of entries in above list
            D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
            &device,                    // returns the Direct3D device created
            &m_featureLevel,            // returns feature level of device created
            &context                    // returns the device immediate context
            );
	if (FAILED(hr))
    {
		GHDebugMessage::outputString("Failed to create d3d device");
    }
	if (adapter)
	{
		adapter->Release();
	}

    // Get the DirectX11.1 device by QI off the DirectX11 one.
    hr = device.As(&m_d3dDevice);
	if (FAILED(hr)) {
		GHDebugMessage::outputString("Failed to get d3d 11.1 off 11");
	}

    // And get the corresponding device context in the same way.
    hr = context.As(&m_d3dContext);
	if (FAILED(hr)) {
		GHDebugMessage::outputString("Failed to get d3d context");
	}

#if defined(_DEBUG)
	ID3D11Debug* d3dDebug = nullptr;
	if (SUCCEEDED(m_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
	{
		ID3D11InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
			
			D3D11_MESSAGE_ID hide[] =
			{
				// We're not always setting a sampler in data when the results will be nulled out anyway.
				// Example: reflection map when reflection intensity is 0.
				D3D11_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET,
				D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
				// these two let us bind a depth target that is already bound to a shader target.
				D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
				// Add more message IDs here as needed
			};

			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
			d3dInfoQueue->Release();
		}
		d3dDebug->Release();
	}
#endif
}

void GHRenderDeviceDX11Native::createWindowSizeDependentResources()
{
	HRESULT res;

	int width = convertDipsToPixels((float)mScreenInfo.getSize()[0]);
	int height = convertDipsToPixels((float)mScreenInfo.getSize()[1]);
	// we used to draw at half ratio for graphics quality 0 by /=2 on width and height.
	// this was mainly for the very weak early gen tablets.
	width = width - 1;
	height = height - 1;

    // If the swap chain already exists, resize it.
    if(mSwapChain != nullptr)
    {
		GHDebugMessage::outputString("Resizing to %d %d", width, height);
        res = mSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (FAILED(res)) {
			GHDebugMessage::outputString("Failed to resize swap chain");
		}
	}
    // Otherwise, create a new one.
    else
    {
		// Once the desired swap chain description is configured, it must be created on the same adapter as our D3D Device

// First, retrieve the underlying DXGI Device from the D3D Device
		Microsoft::WRL::ComPtr<IDXGIDevice1>  dxgiDevice;
		res = m_d3dDevice.As(&dxgiDevice);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to get dxgiDevice");

		// Identify the physical adapter (GPU or card) this device is running on.
		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
		res = dxgiDevice->GetAdapter(&dxgiAdapter);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to get dxgiAdapter");

		mMSAACount = mDesiredMSAA;

		mSwapChainCreator.createSwapChain(width, height, m_d3dDevice, dxgiDevice, dxgiAdapter, mSwapChain, mMSAACount);
    }
    
    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    res = mSwapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            &backBuffer
            );
	if (FAILED(res)) GHDebugMessage::outputString("Failed to get backbuffer");

    // Create a view interface on the rendertarget to use on bind.
    res = m_d3dDevice->CreateRenderTargetView(
            backBuffer.Get(),
            nullptr,
            &m_renderTargetView
            );
	if (FAILED(res)) GHDebugMessage::outputString("Failed to create render target view");

	mActiveRenderTargetView = m_renderTargetView;

    // Cache the rendertarget dimensions in our helper class for convenient use.
    D3D11_TEXTURE2D_DESC backBufferDesc;
    backBuffer->GetDesc(&backBufferDesc);
    m_renderTargetSize[0]  = static_cast<float>(backBufferDesc.Width);
    m_renderTargetSize[1] = static_cast<float>(backBufferDesc.Height);

	GHRenderDeviceDX11::createWindowSizeDependentResources();
}

GHTexture* GHRenderDeviceDX11Native::resolveBackbuffer(void)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT res = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	if (FAILED(res)) {
		GHDebugMessage::outputString("Failed to get backbuffer in GHRenderDeviceDX11Native::resolveBackbuffer");
		return nullptr;
	}

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	if (backBufferDesc.SampleDesc.Count > 1)
	{
		UINT support = 0;
		res = getD3DDevice()->CheckFormatSupport(backBufferDesc.Format, &support);
		if (FAILED(res) || !(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)) {
			GHDebugMessage::outputString("Unable to resolve backbuffer: Device does not support multisample resolve");
			return nullptr;
		}
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> resolvedTex;
	backBufferDesc.SampleDesc.Count = 1;
	backBufferDesc.SampleDesc.Quality = 0;
	res = getD3DDevice()->CreateTexture2D(&backBufferDesc, nullptr, resolvedTex.GetAddressOf());
	if (FAILED(res)) {
		GHDebugMessage::outputString("Unable to create texture to resolve backbuffer to");
		return nullptr;
	}

	getD3DContext()->ResolveSubresource(resolvedTex.Get(), 0, backBuffer.Get(), 0, backBufferDesc.Format);

	GHTextureDX11* ret = new GHTextureDX11(*this, resolvedTex, nullptr,
										   nullptr, nullptr, nullptr, nullptr, 
										   backBufferDesc.Width,
										   backBufferDesc.Height, 
										   (unsigned int)GHDXGIUtil::bitsPerPixel(backBufferDesc.Format) / 8,
										   backBufferDesc.MipLevels > 1,
										   backBufferDesc.Format);
	return ret;
}

// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
int GHRenderDeviceDX11Native::convertDipsToPixels(float dips)
{
#ifdef WIN32
	return (int)dips;
#else
    static const float dipsPerInch = 96.0f;
	// Round to nearest integer.
    return (int)floor(dips * Windows::Graphics::Display::DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); 
#endif
}
