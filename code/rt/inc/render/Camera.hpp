#pragma once
#include <d3d11.h>
#include <xnamath.h>

namespace rt
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void setView(const XMMATRIX& view) { m_view = view; };
		void setProjection(const XMMATRIX& projection) { m_projection = projection; };

		const XMMATRIX& getView() const { return m_view; }
		const XMMATRIX& getProjection() const { return m_projection; }
		
		void setPosition() {} // TODO: implement;
		void setRotation() {} // TODO: implement;

	private:
		XMMATRIX m_projection;
		XMMATRIX m_view;
	};
}