#include "system/Resources.h"

namespace rt
{

Resources::Resources()
{

}


Resources::~Resources()
{

}

bool Resources::init(system::ConfigRef config)
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT res = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &m_dx11Device, &featureLevel, &m_dx11Context);
	return true;
}

void Resources::deinit()
{
	Release(m_dx11Context);
	Release(m_dx11Device);
}

}
