#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

#include "System.hpp"

namespace rt
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

	class Resources
	{
	public:
		Resources();
		~Resources();

		bool init(system::ConfigRef config);
		void deinit();

		ID3D11Device* getDxDevice() { return m_dx11Device; }
		ID3D11DeviceContext* getContext() { return m_dx11Context; }
	private:
		ID3D11Device* m_dx11Device = nullptr;
		ID3D11DeviceContext* m_dx11Context = nullptr;
	};
}