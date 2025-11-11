module;

#include <memory>

module App;

import :PlayingGameState;
import :GameState;

import DirectGL;

class TheTower : public DGL::Sketch
{
public:

	bool Setup() override
	{
		DGL::SetWindowSize(1600, 1080);

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

	void Draw(const float deltaTime) override
	{
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