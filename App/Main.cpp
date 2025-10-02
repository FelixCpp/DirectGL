#include <memory>
#include <format>
#include <vector>
#include <filesystem>

import DGL;

struct SpikesGame : DGL::Sketch
{
	std::unique_ptr<DGL::TextureSampler> sampler = DGL::CreateTextureSampler(DGL::TextureWrapMode::ClampToEdge, DGL::TextureFilterMode::Nearest);
	std::unique_ptr<DGL::SolidColorBrush> brush = DGL::CreateSolidColorBrush(DGL::Color(100, 200, 130));
	std::unique_ptr<DGL::Texture> texture = DGL::CreateTexture("test.jpg");
	std::unique_ptr<DGL::Brush> textureBrush = DGL::CreateTextureBrush(*texture, *sampler);

	std::unique_ptr<DGL::OffscreenRenderTarget> renderTarget = DGL::CreateOffscreenRenderTarget(900, 900);
	std::unique_ptr<DGL::TextureBrush> offscreenBrush = DGL::CreateTextureBrush(*renderTarget->GetTexture(), *sampler);

	bool Setup() override
	{
		return true;
	}

	int mx = 0, my = 0;
	bool visible = false;

	void Event(const System::WindowEvent& event) override
	{
		event.Visit(
			[](const System::WindowEvent::Closed&)
			{
				DGL::Info("Window close event received");
			},
			[](const System::WindowEvent::Resized& resized)
			{
				DGL::Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
			},
			[this](const System::WindowEvent::MouseMoved& move)
			{
				mx = move.MouseX;
				my = move.MouseY;
			},
			[](const auto&)
			{
				//DGL::Info("An unknown event has been received");
			}
		);
	}

	void Draw() override
	{
		auto* renderTarget = &DGL::GetRenderTarget();

		const Math::Float2 start = { 450.0f, 450.0f };
		const Math::Float2 end = { (float)mx, (float)my }; //{ 800.0f, 450.0f };

		//brush->SetColor(DGL::Color(0, 0, 0));
		

		//brush->SetColor(DGL::Color(100, 200, 130));
		renderTarget->Line(start, end, 300.0f, *textureBrush, DGL::LineCap::Round, DGL::LineCap::Round);

		if (visible)
		{
			brush->SetColor(DGL::Color(200, 100, 130));
			renderTarget->FillEllipse(start, DGL::Radius::Circular(7.5f), *brush);
			renderTarget->FillEllipse(end, DGL::Radius::Circular(7.5f), *brush);
		}

		renderTarget->End();
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
