#include "world/World.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"

#include "components/Renderable.hpp"
#include "components/Transform.hpp"

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
					initCamera(cameraNode, static_cast<float>(config->windowSizeX) / static_cast<float>(config->windowSizeY));
				}

				if (tinyxml2::XMLNode* scene = root->FirstChildElement("objects"))
				{
					// creating root object
					void* mem = rt::system::System::allocAllignement(sizeof(object::Object), 16); // TODO: improve
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

void World::deinit()
{
	for (auto& it : m_objects)
	{
		delete it;
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
		m_objects.push_back(object);
	}
}

const rt::world::World::Objects& World::getRenderableObjects()
{
	m_renderableObjects.clear();

	for (auto& it : m_objects)
	{
		gatherRenderable(m_renderableObjects, it);
	}

	return m_renderableObjects;
}

void World::initCamera(tinyxml2::XMLNode* node, float aspectRatio)
{
	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;

	float tx = 0.0f;
	float ty = 0.0f;
	float tz = 0.0f;

	float ux = 0.0f;
	float uy = 0.0f;
	float uz = 0.0f;

	float fov = 0.0f;
	float nearZ = 0.0f;
	float farZ = 0.0f;

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
	m_camera.buildProjectionMatrix(aspectRatio, fov, nearZ, farZ, XMVectorSet(px, py, pz, 1.0f), XMVectorSet(ux, uy, uz, 0.0f), XMVectorSet(tx, ty, tz, 0.0f));
}

void World::parseObjects(tinyxml2::XMLNode* child, object::Object* root)
{
	for (child; child; child = child->NextSibling())
	{
		void* mem = rt::system::System::allocAllignement(sizeof(object::Object), 16); // TODO: improve
		object::Object *object = new (mem) object::Object();

		const char* name = child->ToElement()->Attribute("name");
		object->setName(name);

		//parse components
		parseComponents(child->FirstChildElement("components"), object);

		if (root)
		{
			root->addChild(object);
		}
		else
		{
			m_objects.push_back(object);
		}
	
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
		}
	}
}

void World::parseTransformComponent(tinyxml2::XMLNode* node, object::Object* object)
{
	void* mem = rt::system::System::allocAllignement(sizeof(Transform), 16); // TODO: improve
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
	Renderable* renderable = new Renderable();
	object->getCoreComponents().setRenderable(renderable);
}

void World::gatherRenderable(rt::world::World::Objects& renderableObjects, object::Object* root)
{
	if (!root)
	{
		return;
	}

	if (root->getCoreComponents().getRenderable())
	{
		renderableObjects.push_back(root);
	}

	for (auto& it : root->getChilds())
	{
		gatherRenderable(renderableObjects, it);
	}
}

}
}