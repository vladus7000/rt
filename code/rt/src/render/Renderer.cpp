#include "stdafx.h"
#include "render/Renderer.hpp"
#include "system/Resources.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "render/dxUtils.hpp"

#include <string>

namespace rt
{
namespace render
{

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

	createRenderTargets();
	createDepthTarget();

	createDepthStencilState();
	createRasterizerState();
	
	setUpViewports();

	return true;
}

void Renderer::deinit()
{
	ReleaseCOM(m_dx11RenderTargetView);
	ReleaseCOM(m_dx11DepthStencilView);
	ReleaseCOM(m_rasterState);
	ReleaseCOM(m_depthState);
}

void Renderer::renderFrame()
{
	if (auto dx11Device = Resources::getInstance().getDxDevice())
	{
		auto dx11Context = Resources::getInstance().getContext();

		dx11Context->OMSetRenderTargets(1, &m_dx11RenderTargetView, m_dx11DepthStencilView);
		dx11Context->OMSetDepthStencilState(m_depthState, 1);
		dx11Context->RSSetState(m_rasterState);
		dx11Context->RSSetViewports(1, &m_viewport);

		FLOAT color[4] = { 0.4f, 0.5f, 0.4f, 1.0 };
		dx11Context->ClearRenderTargetView(m_dx11RenderTargetView, color);
		dx11Context->ClearDepthStencilView(m_dx11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (m_world)
		{
			//first draw opaque geometry
			RenderableContext context;
			context.clear();

			context.projectionMatrix = m_world->getProjectionMatrix();
			context.viewMatrix = m_world->getViewMatrix();
			context.viewProjectionMatrix = m_world->getViewProjectionMatrix();

			for (const auto& it : m_world->getRenderableObjects())
			{
				auto renderable =  static_cast<Renderable*>(it->getCoreComponents().getRenderable());
				auto transform = static_cast<Transform*>(it->getCoreComponents().getTransform());
				context.worldMatrix = transform->getWorldTransform(it);
				
				renderable->draw(&context);
			}
			//second transparent
			//everething else  
		}

		auto swapChain = Resources::getInstance().getSwapChain();
		swapChain->Present(1, 0);
	}
}

void Renderer::setWorld(world::World* world)
{
	m_world = world;
}

void Renderer::createRenderTargets()
{
	auto dxDevice = Resources::getInstance().getDxDevice();
	auto swapChain = Resources::getInstance().getSwapChain();

	ID3D11Texture2D* backBuffer = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	dxDevice->CreateRenderTargetView(backBuffer, 0, &m_dx11RenderTargetView);
	ReleaseCOM(backBuffer);
}

void Renderer::createDepthTarget()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_windowsX;
	depthStencilDesc.Height = m_windowsY;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	auto dxDevice = Resources::getInstance().getDxDevice();
	ID3D11Texture2D* depthTexture = nullptr;
	dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthTexture);
	dxDevice->CreateDepthStencilView(depthTexture, 0, &m_dx11DepthStencilView);
	ReleaseCOM(depthTexture);
}

void Renderer::createDepthStencilState()
{
	auto dxDevice = Resources::getInstance().getDxDevice();
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dxDevice->CreateDepthStencilState(&dsDesc, &m_depthState);
}

void Renderer::createRasterizerState()
{
	auto dxDevice = Resources::getInstance().getDxDevice();
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = false;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.AntialiasedLineEnable = false;

	dxDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);
}

void Renderer::setUpViewports()
{
	m_viewport.Width = static_cast<FLOAT>(m_windowsX);
	m_viewport.Height = static_cast<FLOAT>(m_windowsY);
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

}
}