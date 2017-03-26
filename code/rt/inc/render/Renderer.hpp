#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <d3dx11effect.h>

#include "system/System.hpp"
#include "world/World.hpp"

namespace rt
{
	namespace render
	{
		class Renderer
		{
		public:
			Renderer();
			~Renderer();

			bool init(system::ConfigRef config);
			void deinit();

			void renderFrame();
			void setWorld(world::World* world);

		private:
			void createRenderTargets();
			void createDepthTarget();
			void createDepthStencilState();
			void createRasterizerState();
			void setUpViewports();

			void bindGbuffer();
			void clearGbuffer();
			void shadeGBuffer();
			void initLightShader();

			void bindScreenRenderTarget();
			void clearScreenRenderTarget();

		private:
			uint32 m_windowsX;
			uint32 m_windowsY;
			static const uint32 m_gbufferCount = 3;

			ID3D11RenderTargetView* m_dx11RenderTargetView = nullptr;
			ID3D11DepthStencilView* m_dx11DepthStencilView = nullptr;
			ID3D11RasterizerState* m_rasterState = nullptr;
			ID3D11DepthStencilState* m_depthState = nullptr;

			ID3D11RenderTargetView* m_gbuffer[m_gbufferCount];
			ID3D11ShaderResourceView* m_gbufferSRV[m_gbufferCount];

			ID3DX11Effect* m_lightEffect;
			ID3D11InputLayout* m_inputLayout;
			ID3D11Buffer* m_quadVertexBuffer;
			ID3D11Buffer* m_quadIndexBuffer;

			D3D11_VIEWPORT m_viewport;
			world::World* m_world = nullptr;
		};
	}
}