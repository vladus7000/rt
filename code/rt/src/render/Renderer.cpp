#include "stdafx.h"
#include "render/Renderer.hpp"
#include "system/Resources.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "render/dxUtils.hpp"
#include "render/Light.hpp"
#include <d3dx11async.h>

#include <string>
#include <iostream>

namespace rt
{
namespace render
{

Renderer::Renderer()
	: m_msaaQualityCount(0)
	, m_msaaQuality(0)
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
	m_msaaQualityCount = config->msaaQualityCount;
	m_msaaQuality = config->msaaQuality;

	for (uint32 i = 0; i < m_gbufferCount; i++)
	{
		m_gbuffer[i] = nullptr;
		m_gbufferSRV[i] = nullptr;
	}
	createRenderTargets();
	createDepthTarget();

	createDepthStencilState();
	createRasterizerState();
	
	setUpViewports();
	initLightShader();

	return true;
}

void Renderer::deinit()
{
	ReleaseCOM(m_dx11RenderTargetView);
	ReleaseCOM(m_dx11DepthStencilView);
	ReleaseCOM(m_rasterState);
	ReleaseCOM(m_depthState);
	ReleaseCOM(m_lightEffect);
	ReleaseCOM(m_inputLayout);
	ReleaseCOM(m_quadVertexBuffer);
	ReleaseCOM(m_quadIndexBuffer);

	for (uint32 i = 0; i < m_gbufferCount; i++)
	{
		ReleaseCOM(m_gbuffer[i]);
		ReleaseCOM(m_gbufferSRV[i]);
	}

}

void Renderer::renderFrame()
{
	if (auto dx11Device = Resources::getInstance().getDxDevice())
	{
		auto dx11Context = Resources::getInstance().getContext();

		dx11Context->OMSetDepthStencilState(m_depthState, 1);
		dx11Context->RSSetState(m_rasterState);
		dx11Context->RSSetViewports(1, &m_viewport);

		if (m_world)
		{
			//first draw opaque geometry to gbuffer
			RenderableContext context;
			context.clear();

			//
			context.depthOnly = true;
			auto& mainCamera = m_world->getMainCamera();
			auto& visibleObjects = m_world->getVisibleItems(mainCamera.getUID());

			for (const auto& light : visibleObjects.lights)
			{
				if (!light->isCastShadow())
				{
					continue;
				}

				auto depthView = light->getDepth();
				dx11Context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				dx11Context->OMSetRenderTargets(0, nullptr, depthView);

				context.projectionMatrix = light->getCamera().getProjection();
				context.viewMatrix = light->getCamera().getView();
				context.viewProjectionMatrix = light->getCamera().getViewProjection();

				for (const auto& it : visibleObjects.objects)
				{
					auto renderable = static_cast<Renderable*>(it->getCoreComponents().getRenderable());
					auto transform = static_cast<Transform*>(it->getCoreComponents().getTransform());
					context.worldMatrix = transform->getWorldTransform(it);

					renderable->draw(&context);
				}
			}
			//
			bindGbuffer();
			clearGbuffer();
			dx11Context->ClearDepthStencilView(m_dx11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			context.clear();
			
			context.projectionMatrix = mainCamera.getProjection();
			context.viewMatrix = mainCamera.getView();
			context.viewProjectionMatrix = mainCamera.getViewProjection();

			context.lightMatrix = visibleObjects.lights[0]->getCamera().getViewProjection();
			for (const auto& it : visibleObjects.objects)
			{
				auto renderable =  static_cast<Renderable*>(it->getCoreComponents().getRenderable());
				auto transform = static_cast<Transform*>(it->getCoreComponents().getTransform());
				context.worldMatrix = transform->getWorldTransform(it);
				
				renderable->draw(&context);
			}
			
			dx11Context->ClearDepthStencilView(m_dx11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			bindScreenRenderTarget();
			clearScreenRenderTarget();
			shadeGBuffer(visibleObjects);

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

	// create Gbufer
	// gbuffer[0] - [diffR, diffG, diffB, tu]
	// gbuffer[1] - [nx, ny, nz, tv]
	// gbuffer[2] - [height, pow, -, -]
	
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_windowsX;
	desc.Height = m_windowsY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	if (m_msaaQualityCount > 1)
	{
		desc.SampleDesc.Count = m_msaaQualityCount;
		desc.SampleDesc.Quality = m_msaaQuality;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

	}
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = desc.Format;
	renderTargetViewDesc.ViewDimension = m_msaaQualityCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = m_msaaQualityCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (uint32 i = 0; i < m_gbufferCount; i++)
	{
		ID3D11Texture2D* texture = nullptr;
		dxDevice->CreateTexture2D(&desc, 0, &texture);
		dxDevice->CreateRenderTargetView(texture, &renderTargetViewDesc, &m_gbuffer[i]);
		dxDevice->CreateShaderResourceView(texture, &srvDesc, &m_gbufferSRV[i]);
		ReleaseCOM(texture);
	}
}

void Renderer::createDepthTarget()
{
	m_dx11DepthStencilView = Resources::getInstance().createDepthBuffer();
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
	rasterDesc.MultisampleEnable = false;
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

void Renderer::bindGbuffer()
{
	auto dx11Context = Resources::getInstance().getContext();
	dx11Context->OMSetRenderTargets(m_gbufferCount, m_gbuffer, m_dx11DepthStencilView);
}

void Renderer::clearGbuffer()
{
	auto dx11Context = Resources::getInstance().getContext();

	FLOAT color[3][4] = { { 0.0f, 0.0f, 0.0f, 0.0 },{ 0.0f, 0.0f, 0.0f, 0.0 },{ 0.0f, 0.0f, 0.0f, 0.0 }};
	for (uint32 i = 0; i < m_gbufferCount; i++)
	{
		dx11Context->ClearRenderTargetView(m_gbuffer[i], color[i]);
	}
}

void Renderer::shadeGBuffer(world::VisibleItems& items)
{
	uint32 stride = sizeof(float32) * 2;
	uint32 offset = 0;

	auto dxContext = Resources::getInstance().getContext();
	dxContext->IASetInputLayout(m_inputLayout);
	dxContext->IASetIndexBuffer(m_quadIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dxContext->IASetVertexBuffers(0, 1, &m_quadVertexBuffer, &stride, &offset);

	ID3DX11EffectTechnique* tech;
	tech = m_lightEffect->GetTechniqueByName("Lighting");
	D3DX11_TECHNIQUE_DESC desc;
	tech->GetDesc(&desc);

	ID3DX11EffectShaderResourceVariable* gbuffer0 = nullptr;
	ID3DX11EffectShaderResourceVariable* gbuffer1 = nullptr;
	ID3DX11EffectShaderResourceVariable* gbuffer2 = nullptr;
	ID3DX11EffectShaderResourceVariable* depth = nullptr;
	ID3DX11EffectMatrixVariable* lightMatrix = nullptr;

	gbuffer0 = m_lightEffect->GetVariableByName("diffuse_tu")->AsShaderResource();
	gbuffer0->SetResource(m_gbufferSRV[0]);

	gbuffer1 = m_lightEffect->GetVariableByName("normal_tv")->AsShaderResource();
	gbuffer1->SetResource(m_gbufferSRV[1]);

	gbuffer2 = m_lightEffect->GetVariableByName("aux")->AsShaderResource();
	gbuffer2->SetResource(m_gbufferSRV[2]);

	depth = m_lightEffect->GetVariableByName("depth")->AsShaderResource();
	depth->SetResource(items.lights[0]->getDepthSRV());

	lightMatrix = m_lightEffect->GetVariableByName("lightMatrix")->AsMatrix();
	lightMatrix->SetMatrix((float*)&items.lights[0]->getCamera().getViewProjection());
	
	for (unsigned int i = 0; i < desc.Passes; i++)
	{
		dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		tech->GetPassByIndex(i)->Apply(0, dxContext);
		dxContext->DrawIndexed(6, 0, 0);
	}
}

void Renderer::initLightShader()
{
	DWORD flags = 0;
	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilerMsg = nullptr;

	auto& resources = Resources::getInstance();
	auto device = resources.getDxDevice();

	D3DX11CompileFromFile(L"data/shaders/lighting.fx", nullptr, 0, 0, "fx_5_0", flags, 0, nullptr, &compiledShader, &compilerMsg, nullptr);

	if (compilerMsg)
	{
		const char* error = (const char*)compilerMsg->GetBufferPointer();
		std::cout << error << std::endl;
		ReleaseCOM(compilerMsg);
	}
	if (compiledShader)
	{
		D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &m_lightEffect);
		ReleaseCOM(compiledShader);
		D3D11_INPUT_ELEMENT_DESC terrainDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3DX11EffectTechnique* technique = nullptr;
		technique = m_lightEffect->GetTechniqueByName("Lighting");
		D3DX11_PASS_DESC pass;
		technique->GetPassByIndex(0)->GetDesc(&pass);

		device->CreateInputLayout(terrainDesc, sizeof(terrainDesc) / sizeof(terrainDesc[0]), pass.pIAInputSignature, pass.IAInputSignatureSize, &m_inputLayout);
	}

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = 8 * sizeof(float32);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	float32 quadVertices[8] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f };

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = quadVertices;

	device->CreateBuffer(&desc, &initData, &m_quadVertexBuffer);

	uint16 quadIndices[6] = { 0, 1, 2, 0, 2, 3 };
	desc.ByteWidth = 6 * sizeof(uint16);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	initData.pSysMem = quadIndices;
	device->CreateBuffer(&desc, &initData, &m_quadIndexBuffer);
}

void Renderer::bindScreenRenderTarget()
{
	auto context = Resources::getInstance().getContext();
	context->OMSetRenderTargets(1, &m_dx11RenderTargetView, m_dx11DepthStencilView);
}

void Renderer::clearScreenRenderTarget()
{
	auto context = Resources::getInstance().getContext();
	FLOAT color[4] = { 0.4f, 0.5f, 0.4f, 1.0 };
	context->ClearRenderTargetView(m_dx11RenderTargetView, color);
}

}
}