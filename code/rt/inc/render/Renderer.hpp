#pragma once
#include "system/System.hpp"
#include "world/World.hpp"
#include <d3d11.h>
#include <D3Dcompiler.h>

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
			unsigned int m_windowsX;
			unsigned int m_windowsY;

			ID3D11Device* m_dx11Device = nullptr;
			ID3D11DeviceContext* m_dx11Context = nullptr;
			IDXGISwapChain* m_dx11swapChain = nullptr;
			ID3D11RenderTargetView* m_dx11RenderTargetView = nullptr;
			ID3D11DepthStencilView* m_dx11DepthStencilView = nullptr;
			ID3D11Texture2D* m_depthStenciltex = nullptr;
			ID3D11RasterizerState* m_rasterizedState = nullptr;
			ID3D11RasterizerState* m_rasterizedStateWire = nullptr;
			world::World* m_world = nullptr;
		};
	}
}