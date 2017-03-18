#pragma once

#include <vector>

namespace rt
{
	namespace object
	{
		class Object;
	}

	class Component
	{
	public:
		virtual ~Component() {};
		virtual void update(float dt) {};
		void setOwner(object::Object* owner) { m_owner = owner; }
	protected:
		object::Object* m_owner = nullptr;
	};

	class Components
	{
	public:
		Components();
		~Components();

		void update(float dt);
		void setRenderable(Component* renderable);
		void setTransform(Component* transform);
		Component* getRenderable() { return m_components[0]; };
		Component* getTransform() { return m_components[1]; };

		std::vector<Component*> m_components;
	};
}