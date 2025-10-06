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
