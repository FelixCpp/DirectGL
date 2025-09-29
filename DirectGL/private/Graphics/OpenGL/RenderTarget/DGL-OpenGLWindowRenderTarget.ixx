// Project Name : DirectGL
// File Name    : DGL-OpenGLWindowRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

module;

#include <memory>

export module DGL:OpenGLWindowRenderTarget;

import :WindowRenderTarget;
import :Renderer;
import :Camera;

namespace DGL
{
	class OpenGLWindowRenderTarget : public WindowRenderTarget
	{
	public:

		static std::unique_ptr<OpenGLWindowRenderTarget> Create(Window& window, Renderer& renderer);

		void Resize(uint32_t width, uint32_t height) override;
		[[nodiscard]] Math::Uint2 GetSize() const override;

		Window& GetTarget() const override;

		void Begin() override;
		void End() override;

		void DrawRectangle(const Math::FloatBoundary& boundary, float thickness, Brush& brush) override;
		void FillRectangle(const Math::FloatBoundary& boundary, Brush& brush) override;

		void DrawEllipse(const Math::Float2& center, Radius radius, float thickness, Brush& brush) override;
		void FillEllipse(const Math::Float2& center, Radius radius, Brush& brush) override;

	private:

		explicit OpenGLWindowRenderTarget(Window& window, Renderer& renderer);

		Window* m_Window;
		Renderer* m_Renderer;
		Camera m_Camera;

	};
}