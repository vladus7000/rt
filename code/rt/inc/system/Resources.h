#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

#include "System.hpp"

namespace rt
{
	class Resources
	{
	public:
		static Resources& getInstance();
		bool init(system::ConfigRef config);
		void deinit();

		ID3D11Device* getDxDevice() { return m_dx11Device; }
		ID3D11DeviceContext* getContext() { return m_dx11Context; }
		IDXGISwapChain* getSwapChain() { return m_dx11swapChain; }
		uint32 checkMultisampleQuality(DXGI_FORMAT format, uint32 samplesCount);
	private:
		Resources();
		~Resources();

	private:
		IDXGISwapChain* m_dx11swapChain = nullptr;
		ID3D11Device* m_dx11Device = nullptr;
		ID3D11DeviceContext* m_dx11Context = nullptr;
	};
}