#include "../code/rt/inc/system/Types.hpp"
#include "EditorWrap.hpp"
#include "../code/rt/inc/render/Material.hpp"

void EditorWrap::init(HWND hWnd)
{
	m_engine = new EngineProxy();
	m_engine->init(hWnd);
}

void EditorWrap::deinit()
{
	m_engine->deinit();
}

void EditorWrap::tick()
{
	m_engine->tick();
}

const char* EditorWrap::getMaterialsName(int i)
{
	return m_engine->getMaterialsName(i);
}

int EditorWrap::getMaterialsCount()
{
	return m_engine->getMaterialsCount();
}

bool EditorWrap::updateMaterial(MaterialWrap^ m)
{
	using namespace Runtime::InteropServices;

	rt::Material* proxy = new rt::Material();
	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getName()).ToPointer();
		proxy->setName(s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getTexture(MaterialWrap::TextureLayers::albedo)).ToPointer();
		proxy->setTexture(rt::Material::TextureLayers::albedo, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getTexture(MaterialWrap::TextureLayers::glosiness)).ToPointer();
		proxy->setTexture(rt::Material::TextureLayers::glosiness, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getTexture(MaterialWrap::TextureLayers::normal)).ToPointer();
		proxy->setTexture(rt::Material::TextureLayers::normal, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getShader(MaterialWrap::Shader::VS)).ToPointer();
		proxy->setShader(rt::Material::Shader::VS, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getShader(MaterialWrap::Shader::PS)).ToPointer();
		proxy->setShader(rt::Material::Shader::PS, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getShader(MaterialWrap::Shader::GS)).ToPointer();
		proxy->setShader(rt::Material::Shader::GS, s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	{
		char* s = (char*)Marshal::StringToHGlobalAnsi(m->getEffectFile()).ToPointer();
		proxy->setEffectFile(s);
		Marshal::FreeHGlobal((IntPtr)s);
	}

	bool ret = m_engine->updateMaterial(proxy);
	proxy->release();
	return ret;
}

MaterialWrap^ EditorWrap::getMaterial(String^ name)
{
	using namespace Runtime::InteropServices;
	rt::Material proxy;

	char* s = (char*)Marshal::StringToHGlobalAnsi(name).ToPointer();
	m_engine->getMaterial((const char*)s, &proxy);
	Marshal::FreeHGlobal((IntPtr)s);

	MaterialWrap^ m = gcnew MaterialWrap();

	m->setName(gcnew String(proxy.getName().c_str()));

	m->setTexture(MaterialWrap::TextureLayers::albedo,  gcnew String(proxy.getTexture(rt::Material::TextureLayers::albedo).c_str()));
	m->setTexture(MaterialWrap::TextureLayers::glosiness, gcnew String(proxy.getTexture(rt::Material::TextureLayers::glosiness).c_str()));
	m->setTexture(MaterialWrap::TextureLayers::normal, gcnew String(proxy.getTexture(rt::Material::TextureLayers::normal).c_str()));

	m->setShader(MaterialWrap::Shader::VS, gcnew String(proxy.getShader(rt::Material::Shader::VS).c_str()));
	m->setShader(MaterialWrap::Shader::PS, gcnew String(proxy.getShader(rt::Material::Shader::PS).c_str()));
	m->setShader(MaterialWrap::Shader::GS, gcnew String(proxy.getShader(rt::Material::Shader::GS).c_str()));

	m->setEffectFile(gcnew String(proxy.getEffectFile().c_str()));

	return m;
}

MaterialWrap::MaterialWrap()
{
	m_textures = gcnew array<String^>(static_cast<int>(TextureLayers::max));
	m_shaders = gcnew array<String^>(static_cast<int>(Shader::max));
}

void MaterialWrap::setTexture(TextureLayers layer, String^ name)
{
	if (layer < TextureLayers::max)
	{
		m_textures[static_cast<int>(layer)] = name;
	}
}

String^ MaterialWrap::getTexture(TextureLayers layer)
{
	String^ ret = gcnew String("");

	if (layer < TextureLayers::max)
	{
		ret = m_textures[static_cast<int>(layer)];
	}

	return ret;
}

void MaterialWrap::setShader(Shader shader, String^ name)
{
	if (shader < Shader::max)
	{
		m_shaders[static_cast<int>(shader)] = name;
	}
}

String^ MaterialWrap::getShader(Shader shader)
{
	String^ ret = gcnew String("");

	if (shader < Shader::max)
	{
		ret = m_shaders[static_cast<int>(shader)];
	}

	return ret;
}