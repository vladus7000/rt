#pragma once
#include <vector>

#include "system/System.hpp"
#include "system/Object.hpp"
#include "render/Camera.hpp"

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
			void removeObject(object::Object* object);

			const Objects& getObjects() { return m_objects; }

			const Objects& getRenderableObjects(uint64 cameraID = 0);
			void cleanRenderables();

			const Camera& getMainCamera() const { return m_camera; }
			Camera& getMainCamera() { return m_camera; }

		private:
			void initCamera(tinyxml2::XMLNode* node, float aspectRatio);
			void parseObjects(tinyxml2::XMLNode* node, object::Object* root);
			void parseComponents(tinyxml2::XMLNode* node, object::Object* object);
			void parseTransformComponent(tinyxml2::XMLNode* node, object::Object* object);
			void parseGraphicsComponent(tinyxml2::XMLNode* node, object::Object* object);
			void parseTerrain(tinyxml2::XMLNode* node, object::Object* object);
			void gatherRenderable(Objects& renderableObjects, object::Object* root);
		private:
			Objects m_objects;
			Objects m_renderableObjects;
			Camera m_camera;
		};
	}
}