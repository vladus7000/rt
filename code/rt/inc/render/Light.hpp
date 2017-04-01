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
		Camera& getCamera() { return m_camera; }
		ID3D11DepthStencilView* getDepth() { return m_dx11DepthStencilView; }
		ID3D11ShaderResourceView* getDepthSRV() { return m_depthSRV; }

	private:
		~Light();
		Type m_type;
		bool m_castShadow;
		Camera m_camera;
		ID3D11DepthStencilView* m_dx11DepthStencilView;
		ID3D11ShaderResourceView* m_depthSRV;
	};
}