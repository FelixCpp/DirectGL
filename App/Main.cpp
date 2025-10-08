#include <memory>

import DGL;

struct SpikesGame : DGL::Sketch
{
	bool Setup() override
	{
		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
		
	}

	void Draw() override
	{
		const auto ptr = DGL::Renderer::SolidColorBrush::Create(DGL::Color(255, 0, 0));
	}

	void Destroy() override
	{
	}
};

int main() {
	return DGL::Launch([] { return std::make_unique<SpikesGame>(); });
}
