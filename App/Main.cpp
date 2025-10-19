#include <memory>

import DirectGL;
import DirectGL.Math;

struct DirectGLGame : DGL::Sketch
{
	bool Setup() override
	{
		DGL::SetWindowSize(640, 360);

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{

	}

	void Draw(const float deltaTime) override
	{
		DGL::Background(DGL::Renderer::Color(51, 51, 51));

		DGL::StrokeWeight(40.0f);
		DGL::Line(100.0f, 100.0f, DGL::GetMousePosition().X, DGL::GetMousePosition().Y);
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
