#include "stdafx.h"
#include "world/World.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"

#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "world/Terrain.hpp"
#include <algorithm>

namespace rt
{
namespace world
{

World::World()
{

}

World::~World()
{

}
float g_ar = 0.0f;
bool World::init(system::ConfigRef config)
{
	fs::FileDescriptor::Ref configFile = fs::FileManager::LoadFileSync(config->sceneFile);

	if (configFile->valid)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLError eResult = xmlDoc.Parse(reinterpret_cast<const char*>(configFile->data), configFile->fileSize);

		if (eResult == tinyxml2::XMLError::XML_SUCCESS)
		{
			if (tinyxml2::XMLNode* root = xmlDoc.RootElement())
			{
				if (tinyxml2::XMLNode* cameraNode = root->FirstChildElement("camera"))
				{
					g_ar = static_cast<float32>(config->windowSizeX) / static_cast<float32>(config->windowSizeY);
					readCamera(cameraNode, &m_camera, g_ar);
				}

				if (tinyxml2::XMLNode* scene = root->FirstChildElement("objects"))
				{
					// creating root object
					void* mem = system::System::allocAllignement(sizeof(object::Object), 16); // TODO: improve
					object::Object *root = new (mem) object::Object();
					root->setName("Root");

					m_objects.push_back(root);

					tinyxml2::XMLNode* objectNode = scene->FirstChildElement("object");
					parseObjects(objectNode, root);
				}
			}
		}
	}
	return true;
}

void clean(object::Object* o)
{

}

void World::deinit()
{
	cleanVisibleItems();

	for (auto& it : m_objects)
	{
		it->release();
	}
	m_objects.clear();
}

void World::update(float delta)
{
	for (auto& it : m_objects)
	{
		if (it)
		{
			it->update(delta);
		}
	}
}

void World::addObject(object::Object* object)
{
	if (object)
	{
		object->acquire();
		m_objects.push_back(object);
	}
}

void World::removeObject(object::Object* object)
{
	auto foundIT = std::find_if(m_objects.begin(), m_objects.end(), [=](object::Object* o)
	{
		return o == object;
	});

	if (foundIT != m_objects.end())
	{
		m_objects.erase(foundIT);
		object->release();
	}
}

VisibleItems& World::getVisibleItems(uint64 cameraID)
{
	cleanVisibleItems();

	for (auto& it : m_objects)
	{
		gatherVisibleObjects(m_visibleItems, it);
	}

	return m_visibleItems;
}

void World::cleanVisibleItems()
{
	for (auto& it : m_visibleItems.objects)
	{
		it->release();
	}
	for (auto& it : m_visibleItems.lights)
	{
		it->release();
	}

	m_visibleItems.objects.clear();
	m_visibleItems.lights.clear();
}

void World::readCamera(tinyxml2::XMLNode* node, Camera* camera, float aspectRatio)
{
	float32 px = 0.0f;
	float32 py = 0.0f;
	float32 pz = 0.0f;
		 
	float32 tx = 0.0f;
	float32 ty = 0.0f;
	float32 tz = 0.0f;
		 
	float32 ux = 0.0f;
	float32 uy = 0.0f;
	float32 uz = 0.0f;
		 
	float32 fov = 0.0f;
	float32 nearZ = 0.0f;
	float32 farZ = 0.0f;

	if (tinyxml2::XMLElement* position = node->FirstChildElement("position"))
	{
		position->QueryFloatAttribute("x", &px);
		position->QueryFloatAttribute("y", &py);
		position->QueryFloatAttribute("z", &pz);
	}

	if (tinyxml2::XMLElement* target = node->FirstChildElement("target"))
	{
		target->QueryFloatAttribute("x", &tx);
		target->QueryFloatAttribute("y", &ty);
		target->QueryFloatAttribute("z", &tz);
	}

	if (tinyxml2::XMLElement* up = node->FirstChildElement("up"))
	{
		up->QueryFloatAttribute("x", &ux);
		up->QueryFloatAttribute("y", &uy);
		up->QueryFloatAttribute("z", &uz);
	}

	if (tinyxml2::XMLElement* perspective = node->FirstChildElement("perspective"))
	{
		perspective->QueryFloatAttribute("fovAngleY", &fov);
		perspective->QueryFloatAttribute("near", &nearZ);
		perspective->QueryFloatAttribute("far", &farZ);
	}
	camera->buildProjectionMatrix(aspectRatio, fov, nearZ, farZ, XMVectorSet(px, py, pz, 1.0f), XMVectorSet(ux, uy, uz, 0.0f), XMVectorSet(tx, ty, tz, 0.0f));
}

void World::parseObjects(tinyxml2::XMLNode* child, object::Object* root)
{
	for (child; child; child = child->NextSibling())
	{
		const char* name = child->ToElement()->Attribute("name");
		const char* type = child->ToElement()->Attribute("type");

		object::Object *object = nullptr;
		if (type == nullptr)
		{
			void* mem = system::System::allocAllignement(sizeof(object::Object), 16); // TODO: improve
			object = new (mem) object::Object();
		}
		else
		{
			void* mem = system::System::allocAllignement(sizeof(Light), 16); // TODO: improve
			object = new (mem) Light();
			static_cast<Light*>(object)->setCastShadow(true);
		}
		object->setName(name);

		//parse components
		parseComponents(child->FirstChildElement("components"), object);

		if (root)
		{
			root->addChild(object);
		}
		else
		{
			addObject(object);
		}

		object->release();

		parseObjects(child->FirstChildElement("object"), object);
	}
}

void World::parseComponents(tinyxml2::XMLNode* node, object::Object* object)
{
	if (node)
	{
		node = node->FirstChild();
		for (node; node; node = node->NextSibling())
		{
			const char* name = node->ToElement()->Name();
			if (!strcmp(name, "transform"))
			{
				parseTransformComponent(node, object);
			}
			else if (!strcmp(name, "graphics"))
			{
				parseGraphicsComponent(node, object);
			}
			else if (!strcmp(name, "terrain"))
			{
				parseTerrain(node, object);
			}
			else if (!strcmp(name, "camera"))
			{
				readCamera(node, &((Light*)object)->getCamera(), g_ar);
			}
		}
	}
}

void World::parseTransformComponent(tinyxml2::XMLNode* node, object::Object* object)
{
	void* mem = system::System::allocAllignement(sizeof(Transform), 16); // TODO: improve
	Transform* transform = new (mem) Transform();

	if (tinyxml2::XMLElement* position = node->FirstChildElement("position"))
	{
		float a = 0.0f;
		float b = 0.0f;
		float c = 0.0f;

		position->QueryFloatAttribute("x", &a);
		position->QueryFloatAttribute("y", &b);
		position->QueryFloatAttribute("z", &c);

		transform->setPosition(XMVectorSet(a, b, c, 1.0f));
	}

	if (tinyxml2::XMLElement* rotation = node->FirstChildElement("rotationEuler"))
	{
		float a = 0.0f;
		float b = 0.0f;
		float c = 0.0f;

		rotation->QueryFloatAttribute("x", &a);
		rotation->QueryFloatAttribute("y", &b);
		rotation->QueryFloatAttribute("z", &c);
		
		a = a / 180.0f * XM_PI;
		b = b / 180.0f * XM_PI;
		c = c / 180.0f * XM_PI;

		transform->setRotationEuler(XMVectorSet(a, b, c, 0.0f));
	}

	if (tinyxml2::XMLElement* scale = node->FirstChildElement("scale"))
	{
		float a = 0.0f;
		float b = 0.0f;
		float c = 0.0f;

		scale->QueryFloatAttribute("x", &a);
		scale->QueryFloatAttribute("y", &b);
		scale->QueryFloatAttribute("z", &c);

		transform->setScale(XMVectorSet(a, b, c, 1.0f));
	}

	object->getCoreComponents().setTransform(transform);
}

void World::parseGraphicsComponent(tinyxml2::XMLNode* node, object::Object* object)
{
	void* mem = system::System::allocAllignement(sizeof(Renderable), 16); // TODO: improve
	Renderable* renderable = new (mem) Renderable();
	
	object->getCoreComponents().setRenderable(renderable);
	renderable->setOwner(object);
}

void World::parseTerrain(tinyxml2::XMLNode* node, object::Object* object)
{
	void* mem = system::System::allocAllignement(sizeof(Terrain), 16); // TODO: improve
	Terrain* renderable = new (mem) Terrain();

	object->getCoreComponents().setRenderable(renderable);
	renderable->setOwner(object);
}

void World::gatherVisibleObjects(VisibleItems& container, object::Object* root)
{
	if (!root)
	{
		return;
	}

	if (root->getCoreComponents().getRenderable())
	{
		root->acquire();
		container.objects.push_back(root);
	}

	if (root->getType() == object::Object::Type::Light)
	{
		root->acquire();
		container.lights.push_back(static_cast<Light*>(root));
	}

	for (auto& it : root->getChilds())
	{
		gatherVisibleObjects(container, it);
	}
}

}
}