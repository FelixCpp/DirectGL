#include <memory>
#include <format>
#include <vector>

import DGL;

struct SpikesGame : DGL::Sketch
{
	std::vector<Math::Float2> points;
	//DGL::SolidColorBrush solidColorBrush = DGL::SolidColorBrush(0.2f, 0.3f, 0.8f);

	bool Setup() override
	{
		return true;
	}

	void Event(const DGL::WindowEvent& event) override
	{
		event.Visit(
			[](const DGL::WindowEvent::Closed&)
			{
				DGL::Info("Window close event received");
			},
			[](const DGL::WindowEvent::Resized& resized)
			{
				DGL::Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
			},
			[this](const DGL::WindowEvent::MousePressed& press)
			{
				points.push_back(Math::Float2(press.MouseX, press.MouseY));
			},
			[](const DGL::WindowEvent::MouseMoved& move)
			{
				DGL::Info(std::format("Mouse moved to: {}, {}", move.MouseX, move.MouseY));
			},
			[](const DGL::WindowEvent::MouseEntered&)
			{
				DGL::Info("Mouse entered the window");
			},
			[](const DGL::WindowEvent::MouseLeft&)
			{
				DGL::Info("Mouse left the window");
			}
			//[](const auto& )
			//{
			//	DGL::Info("An unknown event has been received");
			//}
		);
	}

	void Draw() override
	{

	}

	void Destroy() override
	{

	}
};

int main() {
	return DGL::Launch([]
	{
		return std::make_unique<SpikesGame>();
	});
}
