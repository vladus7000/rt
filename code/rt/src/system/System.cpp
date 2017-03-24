#include "stdafx.h"
#include "system/System.hpp"
#include "tinyxml2/tinyxml2.h"
#include "system/FileManager.hpp"
#include <stdlib.h>

void * operator new(size_t size)
{
	return _aligned_malloc(size, 1);
}

void * operator new[](size_t size)
{
	return _aligned_malloc(size, 1);
}

void operator delete(void * ptr)
{
	_aligned_free(ptr);
}

void operator delete[](void * ptr)
{
	_aligned_free(ptr);
}

namespace rt
{
namespace system
{

bool System::m_running = false;

namespace
{
	static const char* SettingsFile = "/configs/settings.xml";

	static const unsigned int DefaultWidth = 800;
	static const unsigned int DefaultHeight = 600;
	static const unsigned int DefaultRefreshRate = 60;
	static const char* DefaultWindowName = "RenderTest";

	static const char* graphicsElementName = "graphics";
	static const char* windowElementName = "window";
	static const char* cameraElementName = "camera";
	static const char* sceneElementName = "scene";

	static const char* titleAttributetName = "title";
	static const char* fileNameAttributetName = "file";
	static const char* widthAttributetName = "width";
	static const char* heightAttributetName = "height";
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		System::m_running = false;
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

System::System()
	: m_config(std::make_shared<Config>())
{
	fs::FileDescriptor::Ref configFile = fs::FileManager::LoadFileSync(SettingsFile);
	
	m_config->windowSizeX = DefaultWidth;
	m_config->windowSizeY = DefaultHeight;
	m_config->windowsName = DefaultWindowName;
	m_config->fixedFrameRate = DefaultRefreshRate;

	if (configFile->valid)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLError eResult = xmlDoc.Parse(reinterpret_cast<const char*>(configFile->data), configFile->fileSize);

		if (eResult == tinyxml2::XMLError::XML_SUCCESS)
		{
			if (tinyxml2::XMLNode* root = xmlDoc.RootElement())
			{
				if (tinyxml2::XMLElement* graphics = root->FirstChildElement(graphicsElementName))
				{
					parseGraphicsSettings(*graphics);
				}

				if (tinyxml2::XMLElement* scene = root->FirstChildElement(sceneElementName))
				{
					parseSceneSettings(*scene);
				}

				m_config->windowsName += std::string(" : ") += m_config->sceneFile;
			}
		}
	}
}

System::System(ConfigRef newConfig)
	: m_config(newConfig)
{
}

System::~System()
{
}

bool System::init()
{
	std::wstring windowsName(m_config->windowsName.begin(), m_config->windowsName.end());

	TCHAR szAppName[] = TEXT("System");
	WNDCLASS wndclass;

	HINSTANCE instance = GetModuleHandle(NULL);

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = instance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	RegisterClass(&wndclass);

	m_config->hwnd = CreateWindow(szAppName, windowsName.c_str(),
		WS_OVERLAPPEDWINDOW,
		0, 0,
		m_config->windowSizeX, m_config->windowSizeY,
		NULL, NULL, instance, NULL);

	m_running = m_config->hwnd != nullptr;

	ShowWindow(m_config->hwnd, SW_SHOW);
	UpdateWindow(m_config->hwnd);

	return m_running;
}

void System::deinit()
{
	m_running = false;
}

bool System::update(float32 delta)
{
	MSG msg;
	while (PeekMessage(&msg, m_config->hwnd, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return m_running;
}

ConfigRef System::getConfig()
{
	return m_config;
}

void* System::allocAllignement(size_t size, size_t allign)
{
	return _aligned_malloc(size, allign);
}

void System::parseGraphicsSettings(tinyxml2::XMLNode& node)
{
	if (tinyxml2::XMLElement* window = node.FirstChildElement(windowElementName))
	{
		m_config->windowsName = window->Attribute(titleAttributetName);
		window->QueryUnsignedAttribute(widthAttributetName, &m_config->windowSizeX);
		window->QueryUnsignedAttribute(heightAttributetName, &m_config->windowSizeY);
	}
}

void System::parseSceneSettings(tinyxml2::XMLNode& node)
{
	m_config->sceneFile = static_cast<tinyxml2::XMLElement*>(static_cast<tinyxml2::XMLNode*>(&node))->Attribute(fileNameAttributetName);
}

}
}
