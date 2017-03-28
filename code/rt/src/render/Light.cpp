#include "stdafx.h"
#include "render/Light.hpp"

namespace rt
{

Light::Light()
	: m_type(Type::point)
	, m_castShadow(false)
{

}

void Light::setType(Type type)
{
	m_type = type;
}

}
