#include "TeapotObject.hpp"

namespace rt
{
namespace object
{

Teapot::Teapot()
{
	XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	XMMATRIX Rot = XMMatrixRotationY(1.5708f);

	m_world = Rot * trans;

	m_coreComponents.renderable = &m_renderable;
}

Teapot::~Teapot()
{
	m_coreComponents.renderable = nullptr;
}

void Teapot::update(float delta)
{
	m_renderable.update(delta);
}

}
}