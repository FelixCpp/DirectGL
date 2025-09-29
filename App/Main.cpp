#include <memory>
#include <format>
#include <vector>
#include <filesystem>

import DGL;

struct SpikesGame : DGL::Sketch
{
	std::unique_ptr<DGL::TextureSampler> sampler = DGL::CreateTextureSampler(DGL::TextureWrapMode::ClampToEdge, DGL::TextureFilterMode::Nearest);
	std::unique_ptr<DGL::SolidColorBrush> brush = DGL::CreateSolidColorBrush(DGL::Color(100, 200, 130));

	std::unique_ptr<DGL::OffscreenRenderTarget> renderTarget = DGL::CreateOffscreenRenderTarget(100, 100);
	std::unique_ptr<DGL::TextureBrush> offscreenBrush = DGL::CreateTextureBrush(*renderTarget->GetTexture(), *sampler);

	std::vector<Math::Float2> points;

	bool Setup() override
	{
		return true;
	}

	void Event(const DGL::WindowEvent& event) override
	{
		event.Visit(
			[](const DGL::WindowEvent::Closed&)
			{
				DGL::Info("Window close event received");
			},
			[](const DGL::WindowEvent::Resized& resized)
			{
				DGL::Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
			},
			[this](const DGL::WindowEvent::MousePressed& press)
			{
				// Downscale the coordinates to the offscreen render target size
				const auto [mx, my] = press;
				const auto [ww, wh] = DGL::GetWindowSize();
				const auto [tw, th] = renderTarget->GetSize();

				const float scaledX = static_cast<float>(mx) / static_cast<float>(ww) * tw;
				const float scaledY = static_cast<float>(my) / static_cast<float>(wh) * th;

				points.push_back(Math::Float2(scaledX, scaledY));
			},
			[](const DGL::WindowEvent::MouseMoved& move)
			{
				DGL::Info(std::format("Mouse moved to: {}, {}", move.MouseX, move.MouseY));
			},
			[](const DGL::WindowEvent::MouseEntered&)
			{
				DGL::Info("Mouse entered the window");
			},
			[](const DGL::WindowEvent::MouseLeft&)
			{
				DGL::Info("Mouse left the window");
			}
			//[](const auto& )
			//{
			//	DGL::Info("An unknown event has been received");
			//}
		);
	}

	void Draw() override
	{
		renderTarget->Begin();

		const auto [tw, th] = renderTarget->GetSize();
		brush->SetColor(DGL::Color(255, 255, 255));

		for (size_t y = 0; y < th; ++y)
		{
			for (size_t x = 0; x < tw; ++x)
			{
				if (y % 2 == 0 and x % 2 == 0)
					renderTarget->FillRectangle(Math::FloatBoundary::FromLTWH(static_cast<float>(x), static_cast<float>(y), 1.0f, 1.0f), *brush);
			}
		}

		brush->SetColor(DGL::Color(100, 200, 130));
		for (const auto& point : points)
		{
			renderTarget->FillEllipse(point, DGL::Radius::Circular(5.0f), *brush);
		}

		renderTarget->End();

		DGL::GetRenderTarget().Begin();
		DGL::GetRenderTarget().FillRectangle(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, 900.0f, 900.0f), *offscreenBrush);
		DGL::GetRenderTarget().End();
	}

	void Destroy() override
	{
	}
};

int main() {
	return DGL::Launch(
		DGL::GraphicsAPI::OpenGL,
		[] { return std::make_unique<SpikesGame>(); }
	);
}
