#include <memory>

import DirectGL;

struct DirectGLGame : DGL::Sketch
{
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
		DGL::Background(DGL::Color(21, 21, 21));
		DGL::Fill(DGL::Color(255, 0, 0));
		DGL::Stroke(DGL::Color(0, 255, 0));
		DGL::StrokeWeight(3.0f);
		DGL::SetRectMode(DGL::RectModeCenterWH());
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
