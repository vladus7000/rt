#include "RTProxy.hpp"
#include <vector>
#include <string>

using namespace System;

public ref class MaterialWrap
{
public:
	MaterialWrap();
	enum class TextureLayers
	{
		albedo,
		glosiness,
		normal,
		max
	};

	enum class Shader
	{
		VS,
		PS,
		GS,
		max
	};

	void setName(String^ name) { m_name = name; }
	String^ getName() { return m_name; }

	void setTexture(TextureLayers layer, String^ name);
	String^ getTexture(TextureLayers layer);

	void setShader(Shader shader, String^ name);
	String^ getShader(Shader shader);

	void setEffectFile(String^ file) { m_effectFile = file; }
	String^ getEffectFile() { return m_effectFile; }

	String^ m_name;
	array<String^>^ m_textures;
	array<String^>^ m_shaders;
	String^ m_effectFile;
};

public ref class EditorWrap
{
public:
	void init(HWND hWnd);
	void deinit();
	void tick();

	const char* getMaterialsName(int i);
	int getMaterialsCount();
	bool updateMaterial(MaterialWrap^ m);

	MaterialWrap^ getMaterial(String^ name);

protected:
	EngineProxy* m_engine;
};
