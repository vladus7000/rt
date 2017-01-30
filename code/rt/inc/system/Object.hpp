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
			Object();
			virtual ~Object();

			virtual void update(float delta);

			const XMMATRIX& getWorldTransform() { return m_world; }
			void setWorldTransform(const XMMATRIX& transform) { m_world = transform; }

			const CoreComponents& getCoreComponents() const { return m_coreComponents; }

			void addChild(Object* child);
			void removeChild(Object* child);

			void setRoot(Object* root);
			Object* getRoot() { return m_root; }

			void setName(const std::string& name) { m_name = name; };
			const std::string& getName() const { return m_name; }

		protected:
			XMMATRIX m_world;
			CoreComponents m_coreComponents;
			std::vector<Object*> m_childs;
			Object* m_root = nullptr;
			std::string m_name = "dummy";
		};
	}
}