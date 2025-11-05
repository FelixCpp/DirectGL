#include <memory>

import DirectGL;

using namespace DGL;

class DirectGLGame : public Sketch
{
public:

	bool Setup() override
	{
		SetWindowSize(1600, 1080);

		return true;
	}

	void Event(const WindowEvent& event) override
	{
	}

	void Draw(const float deltaTime) override
	{
		SetStrokeColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		Background({ 0.1f, 0.1f, 0.1f, 1.0f });
		Line(800.0f, 540.0f, GetMousePosition().X, GetMousePosition().Y);

		SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		Rect(100.0f, 100.0f, 300.0f, 300.0f);
	}

	void Destroy() override
	{
	}

};

int main()
{
	return Launch([]
	{
		return std::make_unique<DirectGLGame>();
	});
}