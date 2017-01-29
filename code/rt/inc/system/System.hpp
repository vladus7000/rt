#pragma once
#include <memory>
#include <string>
#include <windows.h>

void* operator new (size_t size, size_t allignement);
void* operator new (size_t size);
void* operator new[](size_t size);

void operator delete (void* ptr);
void operator delete[](void* ptr);

namespace rt
{
	namespace system
	{

		struct Config
		{
			unsigned int windowSizeX;
			unsigned int windowSizeY;
			std::string windowsName;
			std::string rootFolder;
			unsigned int fixedFrameRate;

			HWND hwnd;
		};

		using ConfigRef = std::shared_ptr<Config>;

		class System
		{
		public:
			System();
			System(ConfigRef newConfig);
			~System();

			bool init();
			void deinit();
			bool update(float delta);

			ConfigRef getConfig();

			static void* allocAllignement(size_t size, size_t allign);
		private:
			ConfigRef m_config;
			static bool m_running;
			friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		};
	}
}