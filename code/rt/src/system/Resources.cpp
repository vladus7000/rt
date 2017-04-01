#include "stdafx.h"
#include "system/Resources.h"
#include "render/dxUtils.hpp"

namespace rt
{

Resources::Resources()
{

}


Resources::~Resources()
{

}

Resources& Resources::getInstance()
{
	static Resources res;
	return res;
}

bool Resources::init(system::ConfigRef config)
{
	m_config = config;

	int m_windowsX = config->windowSizeX;
	int m_windowsY = config->windowSizeY;

	D3D_FEATURE_LEVEL featureLevel;
	UINT flags = 0;// D3D11_CREATE_DEVICE_DEBUG;
	HRESULT res = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION, &m_dx11Device, &featureLevel, &m_dx11Context);

	DXGI_SWAP_CHAIN_DESC swapChain;

	ZeroMemory(&swapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	if (config->msaaQualityCount > 1)
	{
		swapChain.SampleDesc.Count = config->msaaQualityCount;
		swapChain.SampleDesc.Quality = config->msaaQuality;
	}
	else
	{
		swapChain.SampleDesc.Count = 1;
		swapChain.SampleDesc.Quality = 0;
	}
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.Width = m_windowsX;
	swapChain.BufferDesc.Height = m_windowsY;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.Flags = 0;
	swapChain.OutputWindow = config->hwnd;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Windowed = true;
	swapChain.BufferCount = 1;

	IDXGIDevice *dxgiDevice;
	IDXGIAdapter *dxgiAdaptor;
	IDXGIFactory *dxgiFactory;
	m_dx11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdaptor);
	dxgiAdaptor->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(m_dx11Device, &swapChain, &m_dx11swapChain);

	ReleaseCOM(dxgiFactory);
	ReleaseCOM(dxgiAdaptor);
	ReleaseCOM(dxgiDevice);
	return true;
}

void Resources::deinit()
{
	ReleaseCOM(m_dx11swapChain);
	ReleaseCOM(m_dx11Context);
	ReleaseCOM(m_dx11Device);
}

unsigned int Resources::checkMultisampleQuality(DXGI_FORMAT format, uint32 samplesCount)
{
	uint32 quality = 0;
	m_dx11Device->CheckMultisampleQualityLevels(format, samplesCount, &quality);
	return quality;
}

ID3D11DepthStencilView * Resources::createDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_config->windowSizeX;;
	depthStencilDesc.Height = m_config->windowSizeY;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	
	if (m_config->msaaQualityCount > 1)
	{
		depthStencilDesc.SampleDesc.Count = m_config->msaaQualityCount;
		depthStencilDesc.SampleDesc.Quality = m_config->msaaQuality;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;// DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTexture = nullptr;
	m_dx11Device->CreateTexture2D(&depthStencilDesc, 0, &depthTexture);

	ID3D11DepthStencilView* depthView = nullptr;
	m_dx11Device->CreateDepthStencilView(depthTexture, 0, &depthView);
	ReleaseCOM(depthTexture);
	return depthView;
}

}
