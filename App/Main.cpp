#include <memory>
#include <format>
#include <vector>
#include <filesystem>

import DGL;

struct SpikesGame : DGL::Sketch
{
	std::unique_ptr<DGL::TextureSampler> sampler = DGL::CreateTextureSampler();
	std::unique_ptr<DGL::Texture> texture = DGL::CreateTexture("test.jpg");
	std::unique_ptr<DGL::TextureBrush> textureBrush = DGL::CreateTextureBrush(*texture, *sampler);

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
				points.push_back(Math::Float2(press.MouseX, press.MouseY));
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
		for (const auto& point : points)
		{
			DGL::GetRenderTarget().DrawRectangle(Math::FloatBoundary::FromLTWH(point.X, point.Y, 100.0f, 100.0f), 30.0f, *textureBrush);
		}
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
