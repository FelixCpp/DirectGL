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

	void Event(const DGL::WindowEvent& event) override
	{
	}

	void Draw(const float deltaTime) override
	{
		DGL::SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		DGL::SetStrokeColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		DGL::SetStrokeWeight(10.0f);

		DGL::BeginShape(DGL::ShapeMode::Points);
		DGL::Vertex(100.0f, 100.0f);
		DGL::Vertex(300.0f, 100.0f);
		DGL::Vertex(300.0f, 300.0f);
		DGL::Vertex(100.0f, 300.0f);
		DGL::EndShape();

		DGL::BeginShape(DGL::ShapeMode::Triangles);
		DGL::SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		DGL::Vertex(400.0f, 100.0f);
		DGL::SetFillColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		DGL::Vertex(600.0f, 100.0f);
		DGL::SetFillColor({ 0.0f, 0.0f, 1.0f, 1.0f });
		DGL::Vertex(600.0f, 300.0f);

		DGL::Vertex(400.0f, 100.0f);
		DGL::Vertex(600.0f, 300.0f);
		DGL::Vertex(400.0f, 300.0f);
		DGL::EndShape();
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