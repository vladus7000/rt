#include <windows.h>

#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif

class CLASS_DECLSPEC EngineProxy
{
public:
	EngineProxy(HWND window);
	~EngineProxy();

	void tick();

};