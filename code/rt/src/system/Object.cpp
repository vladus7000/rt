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
	if (renderable)
	{
		delete renderable;
		renderable = nullptr;
	}
}

Object::Object()
{
	m_transform = XMMatrixIdentity();
	m_position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	m_rotation = XMQuaternionIdentity();
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
	XMMATRIX m1 = XMMatrixIdentity();
	XMMATRIX m2 = XMMatrixIdentity();
	XMMATRIX m3 = XMMatrixIdentity();

	m3 = XMMatrixTranslationFromVector(m_position);
	m2 = XMMatrixRotationQuaternion(m_rotation);
	m1 = XMMatrixScalingFromVector(m_scale);

	m_transform = m1 * m2 * m3;

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

const XMMATRIX& Object::getWorldTransform()
{
	XMMATRIX parentsTransform = XMMatrixIdentity();
	Object* root = getRoot();
	while (root)
	{
		parentsTransform = XMMatrixMultiply(parentsTransform, root->getTransform()); // TODO: check!!!!
		root = root->getRoot();
	}
	m_WorldTransform = XMMatrixMultiply(m_transform, parentsTransform); // TODO: check!!!
	return m_WorldTransform;
}

void Object::setPosition(const XMVECTOR & position)
{
	m_position = position;
}

void Object::setRotation(const XMVECTOR & rotation)
{
	m_rotation = rotation;
}

void Object::setScale(const XMVECTOR& scale)
{
	m_scale = scale;
}

void Object::setRotationEuler(const XMVECTOR& rotation)
{
	m_rotation =  XMQuaternionRotationRollPitchYaw(rotation.m128_f32[1], rotation.m128_f32[2], rotation.m128_f32[0]);
}

void Object::setRenderable(Renderable* renderable)
{
	if (m_coreComponents.renderable)
	{
		delete m_coreComponents.renderable;
	}

	m_coreComponents.renderable = renderable;
}

const XMVECTOR& Object::getPosition() const
{
	return m_position;
}

const XMVECTOR& Object::getRotation() const
{
	return m_rotation;
}

}
}