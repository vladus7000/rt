#include "render/Renderer.hpp"
#include "system/Resources.h"
#include "render/Renderable.hpp"
#include <string>

namespace rt
{
namespace render
{

template <typename T>
inline void Release(T &p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	deinit();
}

bool Renderer::init(system::ConfigRef config)
{
	m_windowsX = config->windowSizeX;
	m_windowsY = config->windowSizeY;

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT res = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &m_dx11Device, &featureLevel, &m_dx11Context);

	DXGI_SWAP_CHAIN_DESC swapChain;

	ZeroMemory(&swapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = 0;
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

	Release(dxgiFactory);
	Release(dxgiAdaptor);
	Release(dxgiDevice);

	ID3D11Texture2D *backBuffer;
	m_dx11swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	m_dx11Device->CreateRenderTargetView(backBuffer, 0, &m_dx11RenderTargetView);
	Release(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_windowsX;
	depthStencilDesc.Height = m_windowsY;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_dx11Device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStenciltex);
	m_dx11Device->CreateDepthStencilView(m_depthStenciltex, 0, &m_dx11DepthStencilView);

	m_dx11Context->OMSetRenderTargets(1, &m_dx11RenderTargetView, m_dx11DepthStencilView);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<FLOAT>(m_windowsX);
	viewport.Height = static_cast<FLOAT>(m_windowsY);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_dx11Context->RSSetViewports(1, &viewport);

	return true;
}

void Renderer::deinit()
{
	Release(m_dx11Context);
	Release(m_dx11Device);
	Release(m_dx11swapChain);
	Release(m_dx11RenderTargetView);
	Release(m_dx11DepthStencilView);
	Release(m_depthStenciltex);
	//Release(m_vertexBuffer);
	//Release(m_vertexShader);
	//Release(m_pixelShader);
	//Release(m_inputLayout);
	Release(m_rasterizedState);
	//Release(depthStencilState);
	Release(m_rasterizedStateWire);
	//Release(constantBuffer);
}

static XMMATRIX InverseTranspose(CXMMATRIX M)
{
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

void Renderer::renderFrame()
{
	if (m_dx11Device)
	{
		FLOAT color[4] = { 0.4f, 0.5f, 0.4f, 1.0 };
		m_dx11Context->ClearRenderTargetView(m_dx11RenderTargetView, color);
		m_dx11Context->ClearDepthStencilView(m_dx11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (m_world)
		{
			for (const auto& it : m_world->getObjects())
			{
				RenderableContext context;
				if (auto renderable = it->getCoreComponents().renderable)
				{
					renderable->prepare(m_dx11Device);

					context.clear();
					if (renderable->fillContext(&context))
					{
						m_dx11Context->IASetInputLayout(context.inputLayout);
						m_dx11Context->IASetIndexBuffer(context.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
						m_dx11Context->IASetVertexBuffers(0, 1, &context.vertexBuffer, &context.stride, &context.offset);
						//context->OMSetDepthStencilState(pDSStateNormal, 1);

						D3DX11_TECHNIQUE_DESC desc;
						ID3DX11EffectTechnique* tech;
						tech = context.dxEffect->GetTechniqueByName("ColorTech");
						tech->GetDesc(&desc);

						ID3DX11EffectMatrixVariable* fxViewProj = nullptr;
						ID3DX11EffectMatrixVariable* fxWorld = nullptr;
						ID3DX11EffectMatrixVariable* fxWorldN = nullptr;
						ID3DX11EffectMatrixVariable* fxTextureM = nullptr;
						ID3DX11EffectShaderResourceVariable* SRV = nullptr;

						fxViewProj = context.dxEffect->GetVariableByName("ViewProj")->AsMatrix();
						fxWorld = context.dxEffect->GetVariableByName("World")->AsMatrix();
						fxWorldN = context.dxEffect->GetVariableByName("WorldN")->AsMatrix();
						fxTextureM = context.dxEffect->GetVariableByName("TextureM")->AsMatrix();
						SRV = context.dxEffect->GetVariableByName("tex")->AsShaderResource();

						XMMATRIX worldNorm = InverseTranspose(it->getWorldTransform());

						SRV->SetResource(context.shaderRV);
						fxViewProj->SetMatrix((float*)&m_world->getViewProjectionMatrix());
						fxWorld->SetMatrix((float*)&it->getWorldTransform());
						fxWorldN->SetMatrix((float*)&worldNorm);

						for (unsigned int i = 0; i < desc.Passes; i++)
						{
							m_dx11Context->IASetPrimitiveTopology(context.topology);
							tech->GetPassByIndex(i)->Apply(0, m_dx11Context);
							m_dx11Context->DrawIndexed(context.indexCount, 0, 0);
						}
					}
				}
			}
		}

		/*world = XMMatrixScaling(0.5, 0.5, 0.5);
		dx11Context->UpdateSubresource(constantBuffer, 0, nullptr, &world, 0, 0);

		dx11Context->VSSetConstantBuffers(0, 1, &constantBuffer);
		dx11Context->Draw(3, 0);
		*/
		/*world = XMMatrixTranslation(-0.5, 0.0, 0.0) * XMMatrixRotationZ(XMConvertToRadians(45.0f)) * XMMatrixScaling(0.5, 0.5, 0.5);
		dx11Context->UpdateSubresource(constantBuffer, 0, nullptr, &world, 0, 0);
		dx11Context->VSSetConstantBuffers(0, 1, &constantBuffer);
		dx11Context->Draw(3, 0);
		*/
		m_dx11swapChain->Present(1, 0);
	}
}

void Renderer::setWorld(world::World* world)
{
	m_world = world;
}

}
}