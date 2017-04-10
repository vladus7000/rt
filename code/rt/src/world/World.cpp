#include "stdafx.h"
#include "world/World.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"

#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "world/Terrain.hpp"

#include <algorithm>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

namespace
{
	inline XMMATRIX getXMMatrix(const aiMatrix4x4& m)
	{
		return	XMMatrixTranspose( XMMATRIX (m.a1, m.a2, m.a3, m.a4,
						  m.b1, m.b2, m.b3, m.b4,
						  m.c1, m.c2, m.c3, m.c4,
						  m.d1, m.d2, m.d3, m.d4));
	}
}

bool World::init(system::ConfigRef config)
{
	fs::FileDescriptor::Ref configFile = fs::FileManager::LoadFileSync(config->sceneFile);
	g_ar = static_cast<float32>(config->windowSizeX) / static_cast<float32>(config->windowSizeY);

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

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile("data/scene2.fbx", aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Quality);
		if (assimpScene)
		{
			if (assimpScene->HasCameras())
			{
				auto camera = assimpScene->mCameras[0];
				auto cameraNode = assimpScene->mRootNode->FindNode(camera->mName);

				XMMATRIX cameraMatrix = getXMMatrix(cameraNode->mTransformation);

				auto parent = cameraNode->mParent;

				while (parent)
				{
					cameraMatrix = XMMatrixMultiply(getXMMatrix(parent->mTransformation), cameraMatrix);
					parent = parent->mParent;
				}

				auto pos = XMVector4Transform(XMVectorSet(camera->mPosition.x, camera->mPosition.y, camera->mPosition.z, 0.0f), cameraMatrix);
				auto up = XMVector4Transform(XMVectorSet(camera->mUp.x, camera->mUp.y, camera->mUp.z, 0.0f), cameraMatrix);
				auto target = XMVector4Transform(XMVectorSet(camera->mLookAt.x, camera->mLookAt.y, camera->mLookAt.z, 0.0f), cameraMatrix);

				m_camera.buildProjectionMatrix(g_ar, camera->mHorizontalFOV/2.0f, camera->mClipPlaneNear, camera->mClipPlaneFar,
					XMVectorSet(camera->mPosition.x, camera->mPosition.y, camera->mPosition.z, 1.0f),
					//XMVectorSet(0, 1, 0, 1.0f),
					XMVectorSet(camera->mUp.x, camera->mUp.y, camera->mUp.z, 1.0f),
					XMVectorSet(camera->mLookAt.x, camera->mLookAt.y, camera->mLookAt.z, 1.0f));

			/*	m_camera.buildProjectionMatrix(g_ar, camera->mHorizontalFOV, camera->mClipPlaneNear, camera->mClipPlaneFar,
					pos,
					//XMVectorSet(0, 1, 0, 1.0f),
					up,
					target);*/
				m_camera.setName(camera->mName.C_Str());
			}

			auto rootNode = assimpScene->mRootNode;
			traverseNodes(*rootNode, assimpScene);

			if (assimpScene->HasLights())
			{
				for (int32 i = 0; i < assimpScene->mNumLights; i++)
				{
					auto light = assimpScene->mLights[i];
					auto node = assimpScene->mRootNode->FindNode(light->mName);

					XMMATRIX matrix(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
						node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
						node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
						node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);



					//m_objects.push_back(graphicsObject);
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

void showTextureInformation(const aiMaterial* pMaterial, aiTextureType pType, unsigned int pTextureNumber)
{
	aiString path;
	aiTextureMapping mapping;
	unsigned int uvindex;
	float blend;
	aiTextureOp op;
	aiTextureMapMode mapmode;
	std::cout << "	Information of texture: " << pTextureNumber << "\n";
	if (AI_SUCCESS == pMaterial->GetTexture(pType, pTextureNumber, &path, &mapping, &uvindex, &blend, &op, &mapmode))
	{
		std::cout << "	Path: " << path.data << "\n";
		std::cout << "	UV index: " << uvindex << "\n";
		std::cout << "	Blend: " << blend << "\n";
	}
	else
	{
		std::cout << "	Impossible to get the texture\n";
	}
}

void printMaterialInfo(aiMaterial* material)
{
	std::cout << "-------------------------Material!!!\n";
	aiString name;
	if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, name))
	{
		std::cout << "	Name: " << name.data << "\n";
	}
	aiColor3D color;
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
	{
		std::cout << "	Ambient color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		std::cout << "	Diffuse color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color))
	{
		std::cout << "	Emissive color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_REFLECTIVE, color))
	{
		std::cout << "	Reflective color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
	{
		std::cout << "	Specular color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	float value;
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, value))
	{
		std::cout << "	Shininess: " << value << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
	{
		std::cout << "	Shininess strength: " << value << "\n";
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, color))
	{
		std::cout << "	Transparent color: " << color.r << " " << color.g << " " << color.b << "\n";
	}
	int intValue;
	if (AI_SUCCESS == material->Get(AI_MATKEY_ENABLE_WIREFRAME, intValue))
	{
		if (intValue == 0)
		{
			std::cout << "	Wireframe: Disabled\n";
		}
		else if (intValue == 1)
		{
			std::cout << "	Wireframe: Enabled\n";
		}
		else
		{
			std::cout << "	Wireframe: Unexpected value\n";
		}
	}
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHADING_MODEL, intValue))
	{
		std::cout << "	Shading model: " << intValue << "\n";
	}
	unsigned int aux = material->GetTextureCount(aiTextureType_AMBIENT);
	if (aux > 0)
	{
		std::cout << "	Number of ambient textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_AMBIENT, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_DIFFUSE);
	if (aux > 0)
	{
		std::cout << "	Number of diffuse textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_DIFFUSE, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_DISPLACEMENT);
	if (aux > 0)
	{
		std::cout << "	Number of displacement textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_DISPLACEMENT, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_EMISSIVE);
	if (aux > 0)
	{
		std::cout << "	Number of emissive textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_EMISSIVE, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_HEIGHT);
	if (aux > 0)
	{
		std::cout << "	Number of height textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_HEIGHT, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_LIGHTMAP);
	if (aux > 0)
	{
		std::cout << "	Number of lightmap textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_LIGHTMAP, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_NORMALS);
	if (aux > 0)
	{
		std::cout << "	Number of normals textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_NORMALS, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_OPACITY);
	if (aux > 0)
	{
		std::cout << "	Number of opacity textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_OPACITY, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_REFLECTION);
	if (aux > 0)
	{
		std::cout << "	Number of reflection textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_REFLECTION, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_SHININESS);
	if (aux > 0)
	{
		std::cout << "	Number of shininess textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_SHININESS, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_SPECULAR);
	if (aux > 0)
	{
		std::cout << "	Number of specular textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_SPECULAR, i);
		}
	}
	aux = material->GetTextureCount(aiTextureType_UNKNOWN);
	if (aux > 0)
	{
		std::cout << "	Number of unknown textures: " << aux << "\n";
		for (unsigned int i = 0; i < aux; i++)
		{
			showTextureInformation(material, aiTextureType_UNKNOWN, i);
		}
	}
	std::cout << "-------------------------\n";
}

void World::traverseNodes(aiNode& root, const aiScene* scene)
{
	XMMATRIX fullTransform = getXMMatrix(root.mTransformation);
	auto parent = root.mParent;

	while (parent)
	{
		fullTransform = XMMatrixMultiply(fullTransform, getXMMatrix(parent->mTransformation));
		parent = parent->mParent;
	}
	
	if (root.mNumMeshes > 0)
	{
		for (int32 i = 0; i < root.mNumMeshes; i++)
		{
			uint32 index = root.mMeshes[i];
			auto mesh = scene->mMeshes[index];

			auto graphicsObject = construct<object::Object>(16);

			auto renderable = construct<Renderable>(16);
			graphicsObject->getCoreComponents().setRenderable(renderable);
			renderable->setOwner(graphicsObject);

			auto material = scene->mMaterials[mesh->mMaterialIndex];

			printMaterialInfo(material);
				
			///----------

			struct SimpleVertexFormat
			{
				float pos[3];
				float tcoord[2];
				float norm[3];
			};

			ID3D11Buffer* vertexBuffer = nullptr;
			ID3D11Buffer* indexBuffer = nullptr;
			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vbDesc.ByteWidth = mesh->mNumVertices * sizeof(SimpleVertexFormat);
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = 0;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			std::vector<SimpleVertexFormat> verts;
			verts.reserve(mesh->mNumVertices);
			for (int i = 0; i < mesh->mNumVertices; i++)
			{
				verts.push_back(SimpleVertexFormat());
				verts[i].pos[0] = mesh->mVertices[i].x;
				verts[i].pos[1] = mesh->mVertices[i].y;
				verts[i].pos[2] = mesh->mVertices[i].z;

				verts[i].tcoord[0] = mesh->mTextureCoords[0][i].x;
				verts[i].tcoord[1] = mesh->mTextureCoords[0][i].y;

				verts[i].norm[0] = mesh->mNormals[i].x;
				verts[i].norm[1] = mesh->mNormals[i].y;
				verts[i].norm[2] = mesh->mNormals[i].z;
			}

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = verts.data();

			auto device = Resources::getInstance().getDxDevice();
			device->CreateBuffer(&vbDesc, &initData, &vertexBuffer);

			std::vector<uint16> indices;
			verts.reserve(mesh->mNumFaces * 3);
			for (int32 i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				for (int32 j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			D3D11_BUFFER_DESC ibDesc;
			ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
			ibDesc.ByteWidth = indices.size() * sizeof(uint16);
			ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibDesc.CPUAccessFlags = 0;
			ibDesc.MiscFlags = 0;
			ibDesc.StructureByteStride = 0;

			initData.pSysMem = indices.data();

			device->CreateBuffer(&ibDesc, &initData, &indexBuffer);
			///----------
			renderable->overrideBuffers(vertexBuffer, indexBuffer, indices.size());
			auto transform = construct<Transform>(16);
			graphicsObject->getCoreComponents().setTransform(transform);
			transform->setBaseTransform(fullTransform);

			m_objects.push_back(graphicsObject);
		}
	}

	for (int32 i = 0; i < root.mNumChildren; i++)
	{
		traverseNodes(*root.mChildren[i], scene);
	}
}

}
}