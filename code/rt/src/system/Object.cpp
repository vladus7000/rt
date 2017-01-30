#include "system/Object.hpp"

namespace rt
{
namespace object
{


CoreComponents::CoreComponents()
	: renderable(nullptr)
{
}

CoreComponents::~CoreComponents()
{
}

Object::Object()
{
}

Object::~Object()
{
	for (auto& it : m_childs)
	{
		delete it;
	}
}

void Object::update(float delta)
{
	for (auto& it : m_childs)
	{
		it->update(delta);
	}
}

void Object::addChild(Object* child)
{
	if (child)
	{
		auto foundIt = std::find(m_childs.begin(), m_childs.end(), child);
		if (foundIt == m_childs.end())
		{
			child->setRoot(this);
			m_childs.push_back(child);
		}
	}
}

void Object::removeChild(Object* child)
{
	if (child)
	{
		auto foundIt = std::find(m_childs.begin(), m_childs.end(), child);
		if (foundIt != m_childs.end())
		{
			(*foundIt)->setRoot(nullptr);
			m_childs.erase(foundIt);
		}
	}
}

void Object::setRoot(Object* root)
{
	if (m_root)
	{
		m_root->removeChild(this);
	}
	m_root = root;
}

}
}