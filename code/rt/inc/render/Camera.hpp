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
		void buildProjectionMatrix(float aspect, float fov, float nearZ, float farZ, const XMVECTOR& position, const XMVECTOR& up, const XMVECTOR& target);

		const XMMATRIX& getView() const { return m_view; }
		const XMMATRIX& getViewProjection() const { return m_viewProjection; }
		const XMMATRIX& getProjection() const { return m_projection; }
		
		void setPosition() {} // TODO: implement;
		void setRotation() {} // TODO: implement;

	private:
		XMMATRIX m_projection;
		XMMATRIX m_view;
		XMMATRIX m_viewProjection;
	};
}