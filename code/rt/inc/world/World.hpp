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

			const XMMATRIX& getViewMatrix() { return m_camera.getView(); }
			const XMMATRIX& getViewProjectionMatrix() { return m_camera.getViewProjection(); }
			const XMMATRIX& getProjectionMatrix() { return m_camera.getProjection(); }

		private:
			void initCamera(tinyxml2::XMLNode& node, float aspectRatio);
			void parseObjects(tinyxml2::XMLNode* node, object::Object* root);
		private:
			Objects m_objects;
			Camera m_camera;
		};
	}
}