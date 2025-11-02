#include <memory>

import DirectGL;

class DirectGLGame : public DGL::Sketch
{
public:

	bool Setup() override
	{
		DGL::SetWindowSize(1600, 1080);

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
	}

	void Draw(const float deltaTime) override
	{
		
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
