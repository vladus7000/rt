#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <vector>
#include <string>

#include "components/Components.hpp"

namespace rt
{
	namespace object
	{
		class Object
		{
		public:
			using Objects = std::vector<object::Object*>;
			Object();
			virtual ~Object();

			const Components& getCoreComponents() const { return m_coreComponents; }
			Components& getCoreComponents() { return m_coreComponents; }

			virtual void update(float delta);

			void addChild(Object* child);
			void removeChild(Object* child);

			void setRoot(Object* root);
			Object* getRoot() { return m_root; }
			const Objects& getChilds() const { return m_childs; }

			void setName(const std::string& name) { m_name = name; };
			const std::string& getName() const { return m_name; }

		protected:
			Components m_coreComponents;
			Objects m_childs;
			Object* m_root = nullptr;
			std::string m_name = "dummy";
		};
	}
}