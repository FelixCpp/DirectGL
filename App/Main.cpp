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
		textureBrush->SetSourceRect(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, 4000.0f, 6000.0f));
		//textureBrush->SetSourceRect(Math::FloatBoundary::FromLTWH(1619.0f, 3144.0f, 401.0f, 478.0f));
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
			DGL::GetRenderTarget().FillRectangle(Math::FloatBoundary::FromLTWH(point.X, point.Y, 300.0f, 300.0f), *textureBrush);
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
