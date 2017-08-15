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

#include <functional>
#include <iostream>

std::function<void()> g_func;

void executor()
{
	if (g_func)
	{
		g_func();
	}
}

class Foo
{
public:
	Foo()
	{
		me = 0;
		std::cout << "Foo()" << " me " << me << std::endl;
	}
	~Foo()
	{
		me = -1;
		std::cout << "~Foo()" << " me " << me << std::endl;
	}
	Foo(const Foo& rhs)
	{
		me = rhs.me + 1;
		std::cout << "Foo(const Foo& rhs)" << " me " << me << std::endl;
	}
	Foo(Foo&& rhs)
	{
		me = rhs.me + 1;
		std::cout << "Foo(Foo&& rhs)" << " me " << me << std::endl;
	}
	Foo& operator=(const Foo& rhs)
	{
		me = rhs.me + 1;
		std::cout << "Foo& operator=(const Foo& rhs)" << " me " << me << std::endl;
	}
	Foo& operator=(Foo&& rhs)
	{
		me = rhs.me + 1;
		std::cout << "Foo& operator=(Foo&& rhs)" << " me " << me << std::endl;
	}
	int me;
};

int main()
{
	{
		
		{
			Foo foo;
			g_func = std::move([foo]() {
				std::cout << "auto lambla = [foo]()" << " mm " << foo.me << std::endl;
			});
			 
		}
		
		executor();
	}
	std::function<void()> f;
	g_func = f;
	System system;
	Renderer render;
	World world;

	bool success = rt::fs::FileManager::getInstance().init();
	rt::fs::FileManager::getInstance().setRoot("./data/");

	success &= system.init();
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
	rt::fs::FileManager::getInstance().deinit();

	return 0;
}