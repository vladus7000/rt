#include "stdafx.h"
#include "RTProxy.hpp"

#include "stdafx.h"
#include "system/System.hpp"
#include "render/Renderer.hpp"
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

EngineProxy::EngineProxy(HWND window)
{
	FileManager fileManager;
	fileManager.setRoot("./data/");
	System system;
	Renderer render;
	World world;

	bool success = fileManager.init();
	success &= system.init(window);
	success &= rt::Resources::getInstance().init(system.getConfig());
	success &= render.init(system.getConfig());
	success &= world.init(system.getConfig());

	render.setWorld(&world);

	if (success)
	{
		auto config = system.getConfig();
		float updateDelta = 1.0f / static_cast<float>(config->fixedFrameRate);

		while (system.update(updateDelta))
		{
			world.update(updateDelta);
			render.renderFrame();
		}
	}

	world.deinit();
	render.deinit();
	rt::Resources::getInstance().deinit();
	system.deinit();
	fileManager.deinit();
}

EngineProxy::~EngineProxy()
{
}

void EngineProxy::tick()
{
}
