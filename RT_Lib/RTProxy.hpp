#include <windows.h>
#include <vector>
#include <string>

#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif

namespace rt
{
	class Material;
}

class CLASS_DECLSPEC EngineProxy
{
public:
	EngineProxy();
	~EngineProxy();

	void init(HWND window);
	void deinit();
	void tick();

	const char* getMaterialsName(int i);
	int getMaterialsCount();
	bool updateMaterial(rt::Material* m);
	void getMaterial(const char* name, rt::Material* m);
};