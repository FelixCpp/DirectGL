// Project Name : DirectGL
// File Name    : DGL-Camera.ixx
// Author       : Felix Busch
// Created Date : 2025/09/26

export module DGL:Camera;

import Math;

export namespace DGL
{
	class Camera
	{
	public:

		explicit Camera(const Math::Float2& size);

		void SetSize(const Math::Float2& size);
		[[nodiscard]] const Math::Float2& GetSize() const;

		void SetPosition(const Math::Float2& position);
		void SetZoom(float zoomFactor);

		const Math::Matrix4x4& GetViewProjectionMatrix() const;

	private:

		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();

		Math::Float2 m_Size;
		Math::Float2 m_Position;
		float m_ZoomFactor;

		Math::Matrix4x4 m_ViewMatrix;
		Math::Matrix4x4 m_ProjectionMatrix;
		Math::Matrix4x4 m_ViewProjectionMatrix;

	};
}