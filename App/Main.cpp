#include <memory>

import DirectGL;

static uint8_t* GetData()
{
	constexpr size_t width = 800;
	constexpr size_t height = 800;
	static uint8_t data[width * height * 4];
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			const size_t index = (y * width + x) * 4;
			data[index + 0] = x;
			data[index + 1] = 100;
			data[index + 2] = 50;
			data[index + 3] = 255; // A
		}
	}
	return data;
}

struct DirectGLGame : DGL::Sketch
{
	//std::unique_ptr<DGL::Texture::Texture> texture = DGL::Texture::Texture::Create(DGL::Math::Uint2(800, 800), GetData());
	bool Setup() override
	{
		DGL::SetWindowSize(1600, 900);

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
		
	}

	void Draw(const float deltaTime) override
	{
		DGL::Background(DGL::Renderer::Color(21, 21, 21));

		DGL::SetImageMode(DGL::RectModeCenterWH());
		//DGL::Image(*texture, DGL::GetMousePosition().X, DGL::GetMousePosition().Y, 300.0f, 300.0f);

		DGL::SetRectMode(DGL::RectModeCenterWH());
		DGL::NoFill();
		DGL::Stroke(DGL::Renderer::Color(0, 255, 0));
		DGL::StrokeWeight(3.0f);
		DGL::Rect(DGL::GetMousePosition().X, DGL::GetMousePosition().Y, 300.0f, 300.0f);
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
