#pragma once
#include "system/Object.hpp"
#include "render/Camera.hpp"

namespace rt
{
	class Light : public object::Object
	{
		enum class Type
		{
			spot,
			point,
			directional
		};
	public:
		Light();

		void setType(Type type);
		Type getType() const { return m_type; }

		void setCastShadow(bool cast);
		bool isCastShadow() { return m_castShadow; }

		const Camera& getCamera() const { return m_camera; }
		ID3D11DepthStencilView* getDepth() { return m_dx11DepthStencilView; }

	private:
		~Light();
		Type m_type;
		bool m_castShadow;
		Camera m_camera;
		ID3D11DepthStencilView* m_dx11DepthStencilView;
	};
}