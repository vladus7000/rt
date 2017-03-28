#include "stdafx.h"
#include "system/Object.hpp"
#include "components/Transform.hpp"

namespace rt
{
namespace object
{

Object::Object()
	: m_ObjectType(Type::Typeless)
{

}

Object::~Object()
{
	for (auto& it : m_childs)
	{
		it->release();
	}
}

void Object::update(float32 delta)
{
	m_coreComponents.update(delta);

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
			child->acquire();
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
			child->release();
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