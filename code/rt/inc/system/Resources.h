#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

#include "System.hpp"
#include <vector>
#include <string>
#include <render/Material.hpp>

namespace rt
{
	class Resources
	{
	public:
		static Resources& getInstance();
		bool init(system::ConfigRef config);
		void deinit();

		system::ConfigRef getConfig() { return m_config; }
		ID3D11Device* getDxDevice() { return m_dx11Device; }
		ID3D11DeviceContext* getContext() { return m_dx11Context; }
		IDXGISwapChain* getSwapChain() { return m_dx11swapChain; }
		uint32 checkMultisampleQuality(DXGI_FORMAT format, uint32 samplesCount);
		ID3D11DepthStencilView* createDepthBuffer();
		void loadMaterials(const std::string& path);
		const char* getMaterialsName(int i);
		const Material* getMaterialByName(const char* name) const;
		bool updateMaterial(Material* m);
		void removeMaterial(Material* m);
		int getMaterialsCount();

	private:
		Resources();
		~Resources();

	private:
		IDXGISwapChain* m_dx11swapChain = nullptr;
		ID3D11Device* m_dx11Device = nullptr;
		ID3D11DeviceContext* m_dx11Context = nullptr;
		system::ConfigRef m_config;
		std::vector<Material*> m_materials;
	};
}