#include "world/World.hpp"

namespace rt
{
namespace world
{

World::World()
{

}

World::~World()
{

}

bool World::init(system::ConfigRef config)
{
	float nearPlane = 1.0f;
	float farPlane = 100.0f;

	XMVECTOR pos = XMVectorSet(0.0f, 5.0f, -8.0f, 1.0f);
	XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view = XMMatrixLookAtLH(pos, target, up);

	//0,785398 = 45deg
	m_projection = XMMatrixPerspectiveFovLH(0.785398f, static_cast<float>(config->windowSizeX) / static_cast<float>(config->windowSizeY), nearPlane, farPlane);


	m_viewProjection = m_view * m_projection;
	//XMMATRIX Identity = XMMatrixIdentity();

	return true;
}

void World::deinit()
{
	for (auto& it : m_objects)
	{
		delete it;
	}
	m_objects.clear();
}

void World::update(float delta)
{
	for (auto& it : m_objects)
	{
		it->update(delta);
	}
}

void World::addObject(object::Object* object)
{
	if (object)
	{
		m_objects.push_back(object);
	}
}

}
}