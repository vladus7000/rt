#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <vector>
#include <string>

namespace rt
{
	class Renderable;
	namespace object
	{
		struct CoreComponents
		{
			CoreComponents();
			~CoreComponents();
			Renderable* renderable;
		};

		class Object
		{
		public:
				using Objects = std::vector<object::Object*>;
			Object();
			virtual ~Object();

			const CoreComponents& getCoreComponents() const { return m_coreComponents; }

			virtual void update(float delta);

			void addChild(Object* child);
			void removeChild(Object* child);

			void setRoot(Object* root);
			Object* getRoot() { return m_root; }
			const Objects& getChilds() const { return m_childs; }

			void setName(const std::string& name) { m_name = name; };
			const std::string& getName() const { return m_name; }

			const XMMATRIX& getTransform() { return m_transform; }
			const XMMATRIX& getWorldTransform();
			void setTransform(const XMMATRIX& transform) { m_transform = transform; }

			void setPosition(const XMVECTOR& position);
			void setRotation(const XMVECTOR& rotation);

			/*
			* meaning:
			* x = Roll
			* y = Pitch
			* z = Yaw
			*/
			void setRotationEuler(const XMVECTOR& rotation);

			const XMVECTOR& getPosition() const;
			const XMVECTOR& getRotation() const;

		protected:
			XMVECTOR m_position;
			XMVECTOR m_rotation;
			XMVECTOR m_scale;
			XMMATRIX m_transform;
			XMMATRIX m_WorldTransform;
			CoreComponents m_coreComponents;
			Objects m_childs;
			Object* m_root = nullptr;
			std::string m_name = "dummy";
		};
	}
}