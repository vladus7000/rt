#pragma once

#include <d3d11.h>
#include <xnamath.h>

#include "components/Components.hpp"

namespace rt
{
	namespace object
	{
		class Object;
	}
	class Transform : public Component
	{
	public:
		Transform();

		virtual void update(float dt) override;

		const XMMATRIX& getTransform() const { return m_transform; }
		XMMATRIX& getTransform() { return m_transform; }
		
		const XMMATRIX& getWorldTransform(object::Object* child);

		void setTransform(const XMMATRIX& transform) { m_transform = transform; }

		void setPosition(const XMVECTOR& position);
		void setRotation(const XMVECTOR& rotation);
		void setScale(const XMVECTOR& scale);

		/*
		* meaning:
		* x = Roll
		* y = Pitch
		* z = Yaw
		*/
		void setRotationEuler(const XMVECTOR& rotation);

		const XMVECTOR& getPosition() const;
		const XMVECTOR& getRotation() const;
	private:
		XMVECTOR m_position;
		XMVECTOR m_rotation;
		XMVECTOR m_scale;
		XMMATRIX m_transform;
		XMMATRIX m_WorldTransform;
	};
}