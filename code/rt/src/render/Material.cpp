#include "stdafx.h"
#include "render/Material.hpp"

namespace rt
{

void Material::setTexture(TextureLayers layer, const std::string& name)
{
	if (layer < TextureLayers::max)
	{
		m_textures[static_cast<int16>(layer)] = name;
	}
}

const std::string& Material::getTexture(TextureLayers layer) const
{
	static std::string ret;

	if (layer < TextureLayers::max)
	{
		return m_textures[static_cast<int16>(layer)];
	}

	return ret;
}

void Material::setShader(Shader shader, const std::string & name)
{
	if (shader < Shader::max)
	{
		m_shaders[static_cast<int16>(shader)] = name;
	}
}

const std::string& Material::getShader(Shader shader) const
{
	static std::string ret;

	if (shader < Shader::max)
	{
		return m_shaders[static_cast<int16>(shader)];
	}

	return ret;
}

void Material::setEffectFile(const std::string& file)
{
	m_effectFile = file;
}

void Material::updateFrom(const Material* m)
{
	for (int i = 0; i < static_cast<int16>(TextureLayers::max); i++)
	{
		m_textures[i] = m->m_textures[i];
	}

	for (int i = 0; i < static_cast<int16>(Shader::max); i++)
	{
		m_shaders[i] = m->m_shaders[i];
	}

	m_effectFile = m->m_effectFile;
}

}
