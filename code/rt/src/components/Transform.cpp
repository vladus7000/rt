#include "components/Transform.hpp"
#include "system/Object.hpp"

namespace rt
{
	Transform::Transform()
	{
		m_transform = XMMatrixIdentity();
		m_position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_rotation = XMQuaternionIdentity();
	}

	void Transform::update(float dt)
	{
		XMMATRIX m1 = XMMatrixIdentity();
		XMMATRIX m2 = XMMatrixIdentity();
		XMMATRIX m3 = XMMatrixIdentity();

		m3 = XMMatrixTranslationFromVector(m_position);
		m2 = XMMatrixRotationQuaternion(m_rotation);
		m1 = XMMatrixScalingFromVector(m_scale);

		m_transform = m1 * m2 * m3;

	}

	void Transform::setPosition(const XMVECTOR & position)
	{
		m_position = position;
	}

	void Transform::setRotation(const XMVECTOR & rotation)
	{
		m_rotation = rotation;
	}

	void Transform::setScale(const XMVECTOR& scale)
	{
		m_scale = scale;
	}

	void Transform::setRotationEuler(const XMVECTOR& rotation)
	{
		m_rotation = XMQuaternionRotationRollPitchYaw(rotation.m128_f32[1], rotation.m128_f32[2], rotation.m128_f32[0]);
	}

	const XMVECTOR& Transform::getPosition() const
	{
		return m_position;
	}

	const XMVECTOR& Transform::getRotation() const
	{
		return m_rotation;
	}

	const XMMATRIX& Transform::getWorldTransform(object::Object* child)
	{
		XMMATRIX parentsTransform = XMMatrixIdentity();
		object::Object* root = child->getRoot();
		while (root)
		{
			if (Transform* transform = static_cast<Transform*>(root->getCoreComponents().getTransform()))
			{
				parentsTransform = XMMatrixMultiply(parentsTransform, transform->getTransform()); // TODO: check!!!!
			}
			root = root->getRoot();
		}

		m_WorldTransform = XMMatrixMultiply(m_transform, parentsTransform); // TODO: check!!!

		return m_WorldTransform;
	}


}