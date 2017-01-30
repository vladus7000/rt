#pragma once
#include "system/System.hpp"
#include "system/Object.hpp"
#include "render\Camera.hpp"
#include <vector>

namespace tinyxml2
{
	class XMLNode;
}

namespace rt
{
	namespace world
	{
		class World
		{
		public:
			using Objects = std::vector<object::Object*>;
			World();
			~World();

			bool init(system::ConfigRef config);
			void deinit();

			void update(float delta);

			void addObject(object::Object* object);
			const Objects& getObjects() { return m_objects; }

			const XMMATRIX& getViewMatrix() { return m_view; }
			const XMMATRIX& getViewProjectionMatrix() { return m_viewProjection; }
			const XMMATRIX& getProjectionMatrix() { return m_projection; }

		private:
			void parseCameraSettings(tinyxml2::XMLNode& node);
			void parseObjectsSettings(tinyxml2::XMLNode* node, object::Object* root);
		private:
			Objects m_objects;
			XMMATRIX m_view;
			XMMATRIX m_viewProjection;
			XMMATRIX m_projection;
			Camera m_camera;
		};
	}
}