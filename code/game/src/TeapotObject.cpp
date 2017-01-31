#include "TeapotObject.hpp"

namespace rt
{
namespace object
{

Teapot::Teapot()
{
	setPosition(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
	setRotationEuler(XMVectorSet(0.0f, 0.0f, 3.14f / 2.0f, 0.0f));

	m_coreComponents.renderable = &m_renderable;
}

Teapot::~Teapot()
{
	m_coreComponents.renderable = nullptr;
}

void Teapot::update(float delta)
{
	Object::update(delta);
	m_renderable.update(delta);
}

}
}