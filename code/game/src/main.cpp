#include "system/System.hpp"
#include "render/Renderer.hpp"
#include "system/FileManager.hpp"
#include "world/World.hpp"

#include <iostream>
#include <new>

using namespace rt::system;
using namespace rt::render;
using namespace rt::world;
using namespace rt::object;
using namespace rt::fs;

int main()
{
	FileManager fileManager;
	fileManager.setRoot("./data/");
	System system;
	Renderer render;
	World world;

	bool success = fileManager.init();
	success &= system.init();
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
	system.deinit();
	fileManager.deinit();

	return 0;
}