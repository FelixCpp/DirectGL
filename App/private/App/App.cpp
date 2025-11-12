module;

#include <memory>
#include <string_view>

module App;

import :PlayingGameState;
import :GameState;

import DirectGL;

class TheTower : public DGL::Sketch
{
public:

	const std::unique_ptr<DGL::Font> font = DGL::Font::CreateFromFile("C:\\Windows\\Fonts\\Arial.ttf", 48);
	
	bool Setup() override
	{
		DGL::SetWindowSize(1600, 1080);

		if (not font)
		{
			DGL::Error("Failed to load font.");
			return false;
		}

		g_AppData = {};
		g_AppData.GameStates.Push(std::make_unique<PlayingGameState>());
		return true;
	}

	void Event(const DGL::WindowEvent& event) override
	{
		if (GameState* state = g_AppData.GameStates.Peek())
		{
			state->Event(event);
		}
	}

	DGL::Math::Float2 Scale = DGL::Math::Float2::One;
	DGL::Math::Float2 Center = DGL::Math::Float2::Zero;

	void Draw(const float deltaTime) override
	{
		//// Zoom into the cursor position if the left mouse button is held
		//if (DGL::IsMouseButtonDown(DGL::MouseButton::Left))
		//{
		//	const DGL::Math::Int2 mousePos = DGL::GetMousePosition();
		//	const DGL::Math::Float2 mousePosF = static_cast<DGL::Math::Float2>(mousePos);
		//	const float zoomFactor = 1.0f + deltaTime; // Zoom in by 100% per second
		//	Scale *= zoomFactor;
		//	Center = (Center - mousePosF) * zoomFactor + mousePosF;
		//}
		//
		//// Zoom out if the right mouse button is held
		//if (DGL::IsMouseButtonDown(DGL::MouseButton::Right))
		//{
		//	const DGL::Math::Int2 mousePos = DGL::GetMousePosition();
		//	const DGL::Math::Float2 mousePosF = static_cast<DGL::Math::Float2>(mousePos);
		//	const float zoomFactor = 1.0f / (1.0f + deltaTime); // Zoom out by 100% per second
		//	Scale *= zoomFactor;
		//	Center = (Center - mousePosF) * zoomFactor + mousePosF;
		//}
		//
		//
		//const std::string_view text = "the quick brown fox jumps over the lazy dog";
		//
		//DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });
		//DGL::PushStyle();
		//{
		//	DGL::Translate(Center.X, Center.Y);
		//	DGL::Scale(Scale.X, Scale.Y);
		//	DGL::SetTextSize(100.0f);
		//	DGL::SetFillColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		//	DGL::SetTextFont(font.get());
		//	DGL::Text(text, 100.0f, 100.0f);
		//}
		//DGL::PopStyle();
		//
		//const DGL::Math::Float2 bounds = font->ComputeTextSize(text, 100.0f);
		//DGL::SetFillColorDisabled();
		//DGL::SetStrokeColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		//DGL::SetStrokeWeight(3.0f);
		//DGL::Rect(100.0f, 100.0f, bounds.X, bounds.Y);

		if (GameState* state = g_AppData.GameStates.Peek())
		{
			state->Update(deltaTime);
			state->Show();
		}
	}

	void Destroy() override
	{
		g_AppData.GameStates.Clear();
	}

};

int main()
{
	return DGL::Launch([]
	{
		return std::make_unique<TheTower>();
	});
}