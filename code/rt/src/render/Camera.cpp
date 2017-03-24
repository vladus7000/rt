#include "stdafx.h"
#include "render/Camera.hpp"

namespace rt
{

Camera::Camera()
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

}