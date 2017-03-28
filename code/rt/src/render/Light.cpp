#include "stdafx.h"
#include "render/Light.hpp"

namespace rt
{

Light::Light()
	: m_type(Type::point)
	, m_castShadow(false)
	, m_dx11DepthStencilView(nullptr)
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
			m_dx11DepthStencilView = Resources::getInstance().createDepthBuffer();
		}
		else
		{
			ReleaseCOM(m_dx11DepthStencilView);
		}
	}
}

Light::~Light()
{
	ReleaseCOM(m_dx11DepthStencilView);
}

}
