#include "stdafx.h"
#include "RTProxy.hpp"

#include "system/System.hpp"
#include "render/Renderer.hpp"
#include "render/Material.hpp"
#include "system/FileManager.hpp"
#include "world/World.hpp"
#include "system/Resources.h"

#include <iostream>
#include <new>

using namespace rt::system;
using namespace rt::render;
using namespace rt::world;
using namespace rt::object;
using namespace rt::fs;

rt::system::System system_;
rt::render::Renderer render;
rt::world::World world;


EngineProxy::EngineProxy()
{
}

EngineProxy::~EngineProxy()
{
}

void EngineProxy::init(HWND window)
{
	bool success = rt::fs::FileManager::getInstance().init();
	rt::fs::FileManager::getInstance().setRoot("./data/");
	success &= system_.init(window);
	success &= rt::Resources::getInstance().init(system_.getConfig());
	success &= render.init(system_.getConfig());
	success &= world.init(system_.getConfig());

	render.setWorld(&world);
}

void EngineProxy::deinit()
{
	world.deinit();
	render.deinit();
	rt::Resources::getInstance().deinit();
	system_.deinit();
	rt::fs::FileManager::getInstance().deinit();
}

void EngineProxy::tick()
{
	auto config = system_.getConfig();
	float updateDelta = 1.0f / static_cast<float>(config->fixedFrameRate);

	if (system_.update(updateDelta))
	{
		world.update(updateDelta);
		render.renderFrame();
	}
}

const char* EngineProxy::getMaterialsName(int i)
{
	return rt::Resources::getInstance().getMaterialsName(i);
}

int EngineProxy::getMaterialsCount()
{
	return rt::Resources::getInstance().getMaterialsCount();
}

bool EngineProxy::updateMaterial(rt::Material* m)
{
	return rt::Resources::getInstance().updateMaterial(m);
}

void EngineProxy::getMaterial(const char* name, rt::Material * m)
{
	if (const rt::Material* mat = rt::Resources::getInstance().getMaterialByName(name))
	{
		m->setName(mat->getName());
		m->updateFrom(mat);
	}
}
