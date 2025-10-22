#include <memory>

import DirectGL;
import DirectGL.Math;

struct DirectGLGame : DGL::Sketch
{
	std::unique_ptr<DGL::GraphicsLayer> layer = DGL::CreateGraphics(500, 500);

	bool Setup() override
	{
		DGL::SetWindowSize(1280, 720);

		DGL::PushLayer(layer.get());
		// Render 10 bars of different colors
		DGL::Fill({ 255, 0, 0 });
		DGL::Rect(0.0f, 0.0f, 500.0f, 250.0f);
		DGL::Fill({ 0, 255, 0 });
		DGL::Rect(0.0f, 250.0f, 500.0f, 250.0f);
		DGL::PopLayer();

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{

	}

	void Draw(const float deltaTime) override
	{
		DGL::Background(DGL::Renderer::Color(100, 100, 100));

		if (DGL::IsMouseButtonDown(DGL::MouseButton::Left))
		{
			const auto [mx, my] = DGL::GetMousePosition();

			if (const auto* offscreenLayer = dynamic_cast<DGL::OffscreenGraphicsLayer*>(layer.get()))
			{
				DGL::Image(offscreenLayer->GetRenderTexture(), mx, my, 50.0f, 50.0f);
			}
		}
	}

	void Destroy() override
	{
	}
};

int main()
{
	return DGL::Launch([]
	{
		return std::make_unique<DirectGLGame>();
	});
}
