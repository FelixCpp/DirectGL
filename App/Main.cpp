#include <memory>
#include <array>

import DirectGL;
import DirectGL.Math;

struct DirectGLGame : DGL::Sketch
{
	std::array<DGL::Math::Float2, 120> points{};
	int frameCount = 0;

	bool Setup() override
	{
		DGL::SetWindowSize(640 * 2, 360 * 2);
		DGL::SetBlend(DGL::Blending::BlendModes::Alpha);
		DGL::NoStroke();
		DGL::Fill(DGL::Renderer::Color(255, 255, 255, 153));

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{

	}

	void Draw(const float deltaTime) override
	{
		DGL::Background(DGL::Renderer::Color(51, 51, 51));

		const size_t index = frameCount % points.size();
		points[index] = static_cast<DGL::Math::Float2>(DGL::GetMousePosition());

		for (size_t i = 0; i < points.size(); ++i)
		{
			const size_t n = (index + 1 + i) % points.size();
			DGL::Circle(points[n].X, points[n].Y, i);
		}

		++frameCount;
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
