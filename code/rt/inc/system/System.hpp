#pragma once

#include <memory>
#include <string>
#include <windows.h>

void* operator new (size_t size, size_t allignement);
void* operator new (size_t size);
void* operator new[](size_t size);

void operator delete (void* ptr);
void operator delete[](void* ptr);

namespace tinyxml2
{
	class XMLNode;
}

namespace rt
{
	namespace system
	{

		struct Config
		{
			uint32 windowSizeX;
			uint32 windowSizeY;
			std::string windowsName;
			std::string rootFolder;
			std::string sceneFile;
			uint32 fixedFrameRate;
			uint32 msaaQualityCount;
			uint32 msaaQuality;

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
			bool update(float32 delta);

			ConfigRef getConfig();

			static void* allocAllignement(size_t size, size_t allign);
		private:
			void parseGraphicsSettings(tinyxml2::XMLNode& node);
			void parseSceneSettings(tinyxml2::XMLNode& node);

			ConfigRef m_config;
			static bool m_running;
			friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		};
	}
}