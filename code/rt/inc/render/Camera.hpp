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
		void buildProjectionMatrix(float fov, float nearZ, float farZ, float px, float py, float pz, float upx, float upy, float upz, float tx, float ty, float tz) {} // TODO: implement and refactor

		const XMMATRIX& getView() const { return m_view; }
		const XMMATRIX& getProjection() const { return m_projection; }
		
		void setPosition() {} // TODO: implement;
		void setRotation() {} // TODO: implement;

	private:
		XMMATRIX m_projection;
		XMMATRIX m_view;
	};
}