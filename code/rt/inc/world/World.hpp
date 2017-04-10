#pragma once
#include <vector>

#include "system/System.hpp"
#include "system/Object.hpp"
#include "render/Camera.hpp"
#include "render/Light.hpp"

namespace tinyxml2
{
	class XMLNode;
}

struct aiNode;
struct aiScene;

namespace rt
{
	namespace world
	{
		struct VisibleItems
		{
			std::vector<object::Object*> objects;
			std::vector<Light*> lights;
		};

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

			VisibleItems& getVisibleItems(uint64 cameraID = 0);
			//const Objects& getRenderableObjects(uint64 cameraID = 0);
			void cleanVisibleItems();

			const Camera& getMainCamera() const { return m_camera; }
			Camera& getMainCamera() { return m_camera; }

		private:
			void readCamera(tinyxml2::XMLNode* node, Camera* camera, float aspectRatio);
			void parseObjects(tinyxml2::XMLNode* node, object::Object* root);
			void parseComponents(tinyxml2::XMLNode* node, object::Object* object);
			void parseTransformComponent(tinyxml2::XMLNode* node, object::Object* object);
			void parseGraphicsComponent(tinyxml2::XMLNode* node, object::Object* object);
			void parseTerrain(tinyxml2::XMLNode* node, object::Object* object);
			void gatherVisibleObjects(VisibleItems& container, object::Object* root);
			void traverseNodes(aiNode& root, const aiScene* scene);

		private:
			Objects m_objects;
			VisibleItems m_visibleItems;
			Camera m_camera;
		};
	}
}