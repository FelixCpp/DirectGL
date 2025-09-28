module DGL;

namespace DGL
{
	Camera::Camera(const Math::Float2& size) :
		m_Size(size),
		m_ZoomFactor(1.0f)
	{
		RecalculateProjectionMatrix();
	}

	void Camera::SetSize(const Math::Float2& size)
	{
		m_Size = size;
		RecalculateProjectionMatrix();
	}

	void Camera::SetPosition(const Math::Float2& position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}

	void Camera::SetZoom(const float zoomFactor)
	{
		m_ZoomFactor = zoomFactor;
		RecalculateProjectionMatrix();
	}

	const Math::Matrix4x4& Camera::GetViewProjectionMatrix() const
	{
		return m_ViewProjectionMatrix;
	}

	void Camera::RecalculateViewMatrix()
	{
		m_ViewMatrix = Math::Matrix4x4::Translation(-m_Position.X, -m_Position.Y, 0.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::RecalculateProjectionMatrix()
	{
		float ww = m_Size.X / 2.0f / m_ZoomFactor;
		float wh = m_Size.Y / 2.0f / m_ZoomFactor;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTRB(0.0f, 0.0f, m_Size.X, m_Size.Y), -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}