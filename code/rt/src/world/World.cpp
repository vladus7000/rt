#include "world/World.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"
#include "TeapotObject.hpp"

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
				if (tinyxml2::XMLElement* graphics = root->FirstChildElement("camera"))
				{
					initCamera(*graphics, static_cast<float>(config->windowSizeX) / static_cast<float>(config->windowSizeY));
				}

				if (tinyxml2::XMLNode* scene = root->FirstChildElement("objects"))
				{
					tinyxml2::XMLNode* objectNode = scene->FirstChildElement("object");
					parseObjects(objectNode, nullptr);
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
		it->update(delta);
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

void World::initCamera(tinyxml2::XMLNode& node, float aspectRatio)
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

	if (tinyxml2::XMLElement* position = node.FirstChildElement("position"))
	{
		position->QueryFloatAttribute("x", &px);
		position->QueryFloatAttribute("y", &py);
		position->QueryFloatAttribute("z", &pz);
	}

	if (tinyxml2::XMLElement* target = node.FirstChildElement("target"))
	{
		target->QueryFloatAttribute("x", &tx);
		target->QueryFloatAttribute("y", &ty);
		target->QueryFloatAttribute("z", &tz);
	}

	if (tinyxml2::XMLElement* up = node.FirstChildElement("up"))
	{
		up->QueryFloatAttribute("x", &ux);
		up->QueryFloatAttribute("y", &uy);
		up->QueryFloatAttribute("z", &uz);
	}

	if (tinyxml2::XMLElement* perspective = node.FirstChildElement("perspective"))
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
		const char* name = child->ToElement()->Attribute("name");
		object::Object *object = nullptr;
		if (strcmp(name, "teapot") == 0) // HACK
		{
			void* mem = rt::system::System::allocAllignement(sizeof(object::Teapot), 16); // TODO: improve
			object = new (mem) object::Teapot();
		}
		else
		{
			void* mem = rt::system::System::allocAllignement(sizeof(object::Object), 16); // TODO: improve
			object = new (mem) object::Object();
		}

		object->setName(name);

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

void World::gatherRenderable(rt::world::World::Objects& renderableObjects, object::Object* root)
{
	if (!root)
	{
		return;
	}

	if (root->getCoreComponents().renderable)
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