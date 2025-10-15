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
