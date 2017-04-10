#include "stdafx.h"
#include "render/Camera.hpp"

namespace rt
{

namespace
{
	uint64 g_cameraUIDS = 0;
}

Camera::Camera()
	: m_cameraUID(g_cameraUIDS++)
	, m_name("DummyCamera")
{

}

Camera::~Camera()
{

}

void Camera::buildProjectionMatrix(float aspect, float fov, float nearZ, float farZ, const XMVECTOR& position, const XMVECTOR& up, const XMVECTOR& target)
{
	m_view = XMMatrixLookAtLH(position, target, up);
	m_projection = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
	m_viewProjection = m_view * m_projection;
}

void Camera::recalculate()
{
	m_viewProjection = m_view * m_projection;
}

}