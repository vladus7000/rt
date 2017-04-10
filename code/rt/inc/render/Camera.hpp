#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <string>

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

		XMMATRIX& getView() { return m_view; }
		XMMATRIX& getViewProjection() { return m_viewProjection; }
		XMMATRIX& getProjection() { return m_projection; }

		void recalculate();

		void setPosition() {} // TODO: implement;
		void setRotation() {} // TODO: implement;

		uint64 getUID() const { return m_cameraUID; }

		const std::string& getName() const { return m_name; }
		void setName(const std::string& name) { m_name = name; }

	private:
		XMMATRIX m_projection;
		XMMATRIX m_view;
		XMMATRIX m_viewProjection;
		const uint64 m_cameraUID;
		std::string m_name;
	};
}