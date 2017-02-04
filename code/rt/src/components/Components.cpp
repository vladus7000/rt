#include "components/Components.hpp"

namespace rt
{
	Components::Components()
	{
		m_components.resize(2);
		for (auto &it : m_components)
		{
			it = nullptr;
		}
	}

	Components::~Components()
	{
		for (auto &it : m_components)
		{
			delete it;
		}
		m_components.clear();
	}

	void Components::update(float dt)
	{
		for (auto &it : m_components)
		{
			if (it)
			{
				it->update(dt);
			}
		}
	}

	void Components::setRenderable(Component* renderable)
	{
		if (m_components[0])
		{
			delete m_components[0];
		}
		m_components[0] = renderable;
	}

	void Components::setTransform(Component* transform)
	{
		if (m_components[1])
		{
			delete m_components[1];
		}
		m_components[1] = transform;
	}
}