#include "stdafx.h"
#include "render/Light.hpp"

namespace rt
{

Light::Light()
	: m_type(Type::point)
	, m_castShadow(false)
	, m_dx11DepthStencilView(nullptr)
	, m_depthSRV(nullptr)
{
	m_ObjectType = Object::Type::Light;
}

void Light::setType(Type type)
{
	m_type = type;
}

void Light::setCastShadow(bool cast)
{
	if (m_castShadow != cast)
	{
		m_castShadow = cast;
		if (m_castShadow)
		{
			auto dxDevice = Resources::getInstance().getDxDevice();
			auto config = Resources::getInstance().getConfig();
			m_dx11DepthStencilView = nullptr;// Resources::getInstance().createDepthBuffer();

			D3D11_TEXTURE2D_DESC depthStencilDesc;
			depthStencilDesc.Width = config->windowSizeX;
			depthStencilDesc.Height = config->windowSizeY;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			if (config->msaaQualityCount > 1)
			{
				depthStencilDesc.SampleDesc.Count = config->msaaQualityCount;
				depthStencilDesc.SampleDesc.Quality = config->msaaQuality;
			}
			else
			{
				depthStencilDesc.SampleDesc.Count = 1;
				depthStencilDesc.SampleDesc.Quality = 0;
			}
			depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depthStencilDesc.CPUAccessFlags = 0;
			depthStencilDesc.MiscFlags = 0;

			ID3D11Texture2D* depthTexture = nullptr;
			dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthTexture);

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.ViewDimension = config->msaaQualityCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

			dxDevice->CreateDepthStencilView(depthTexture, &dsvDesc, &m_dx11DepthStencilView);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.ViewDimension = config->msaaQualityCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			
			dxDevice->CreateShaderResourceView(depthTexture, &srvDesc, &m_depthSRV);


			ReleaseCOM(depthTexture);

		}
		else
		{
			ReleaseCOM(m_dx11DepthStencilView);
			ReleaseCOM(m_depthSRV);
		}
	}
}

Light::~Light()
{
	ReleaseCOM(m_dx11DepthStencilView);
	ReleaseCOM(m_depthSRV);
}

}
