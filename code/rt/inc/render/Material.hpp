#pragma once

#include <string>
#include <vector>
#include "RefCounted.hpp"

namespace rt
{
	class CLASS_DECLSPEC Material : public RefCounted
	{
	public:
		enum class TextureLayers// : rt::int16
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

		void setName(const std::string& name) { m_name = name; }
		const std::string& getName() const { return m_name; }

		void setFileName(const std::string& name) { m_materialFile = name; }
		const std::string& getFileName() const { return m_materialFile; }

		void setTexture(TextureLayers layer, const std::string& name);
		const std::string& getTexture(TextureLayers layer) const;

		void setShader(Shader shader, const std::string& name);
		const std::string& getShader(Shader shader) const;

		void setEffectFile(const std::string& file);
		const std::string& getEffectFile() const { return m_effectFile; };

		void updateFrom(const Material* m);

	private:
		std::string m_textures[static_cast<int16>(TextureLayers::max)];
		std::string m_shaders[static_cast<int16>(Shader::max)];
		std::string m_name;
		std::string m_effectFile;
		std::string m_materialFile;
	};
}