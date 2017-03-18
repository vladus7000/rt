#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

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

		private:
			unsigned int m_windowsX;
			unsigned int m_windowsY;

			ID3D11RenderTargetView* m_dx11RenderTargetView = nullptr;
			ID3D11DepthStencilView* m_dx11DepthStencilView = nullptr;
			ID3D11RasterizerState* m_rasterState = nullptr;
			ID3D11DepthStencilState* m_depthState = nullptr;

			D3D11_VIEWPORT m_viewport;
			world::World* m_world = nullptr;
		};
	}
}