#include "GHDX11SwapChainCreator.h"
#include "GHPlatform/GHDebugMessage.h"

bool GHDX11SwapChainCreator::createSwapChain(int width, int height, 
											 Microsoft::WRL::ComPtr<ID3D11Device1>& d3dDevice,
											 Microsoft::WRL::ComPtr<IDXGIDevice1>& dxgiDevice, Microsoft::WRL::ComPtr<IDXGIAdapter>& dxgiAdapter,
											 Microsoft::WRL::ComPtr<IDXGISwapChain1>& outSwapChain, unsigned int& inOutMSAA)
{
    // Create a descriptor for the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

	if (inOutMSAA > 1)
	{
		uint32_t testQuality = 0;
		if (!SUCCEEDED(d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &testQuality)))
		{
			if (SUCCEEDED(d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 2, &testQuality)))
			{
				inOutMSAA = 2;
			}
			else
			{
				inOutMSAA = 1;
			}
		}
	}

	if (inOutMSAA > 1)
	{
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;           // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 4; // MSAA, crashes
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;                               // use two buffers to enable flip effect
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // necessary for msaa
		swapChainDesc.Flags = 0;
	}
	else
	{
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;           // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;                          // don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;                               // use two buffers to enable flip effect
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // we recommend using this swap effect for all applications
		swapChainDesc.Flags = 0;
	}

	DXGI_ADAPTER_DESC adapterDesc;
	dxgiAdapter->GetDesc(&adapterDesc);
	GHDebugMessage::outputString("Creating swap chaing on %ls", adapterDesc.Description);

    // And obtain the factory object that created it.
    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
    HRESULT res = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);
	if (FAILED(res)) GHDebugMessage::outputString("Failed to get dxgiFactory");

	outSwapChain = createSwapChain(dxgiFactory.Get(), d3dDevice.Get(), swapChainDesc);

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces 
    // latency and ensures that the application will only render after each VSync, minimizing 
    // power consumption.
    res = dxgiDevice->SetMaximumFrameLatency(1);
	if (FAILED(res)) GHDebugMessage::outputString("Failed to set maximum frame latency");

	return true;
}