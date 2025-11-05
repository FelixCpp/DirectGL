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
		const auto [mx, my] = GetMousePosition();
		Background({ 0.1f, 0.1f, 0.1f, 1.0f });
		SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		Circle(mx, my, 50.0f);
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