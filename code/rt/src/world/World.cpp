#include "world/World.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"
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
					parseCameraSettings(*graphics);
				}

				if (tinyxml2::XMLNode* scene = root->FirstChildElement("objects"))
				{
					tinyxml2::XMLNode* objectNode = scene->FirstChildElement("object");
					parseObjectsSettings(objectNode, nullptr);
				}
			}
		}
	}

	float nearPlane = 1.0f;
	float farPlane = 100.0f;

	XMVECTOR pos = XMVectorSet(0.0f, 5.0f, -8.0f, 1.0f);
	XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view = XMMatrixLookAtLH(pos, target, up);

	//0,785398 = 45deg
	m_projection = XMMatrixPerspectiveFovLH(0.785398f, static_cast<float>(config->windowSizeX) / static_cast<float>(config->windowSizeY), nearPlane, farPlane);


	m_viewProjection = m_view * m_projection;
	//XMMATRIX Identity = XMMatrixIdentity();

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

void World::parseCameraSettings(tinyxml2::XMLNode& node)
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
	m_camera.buildProjectionMatrix(fov, nearZ, farZ, px, py, pz, ux, uy, uz, tx, ty, tz);
}

void World::parseObjectsSettings(tinyxml2::XMLNode* child, object::Object* root)
{
	for (child; child; child = child->NextSibling())
	{
		object::Object *object = new object::Object();
		
		if (root)
		{
			root->addChild(object);
		}
		else
		{
			m_objects.push_back(object);
		}

		object->setName(child->ToElement()->Attribute("name"));

		parseObjectsSettings(child->FirstChildElement("object"), object);
	}
}

}
}