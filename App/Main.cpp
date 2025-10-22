#include <memory>

import DirectGL;
import DirectGL.Math;
import DirectGL.Texture;

struct DirectGLGame : DGL::Sketch
{
	std::unique_ptr<DGL::Texture::Texture> texture = DGL::Texture::Texture::Create("wallpaper.jpg");

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
		DGL::Background(DGL::Renderer::Color(0, 0, 0));
		
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
