module DirectGL;

import :InputListener;

namespace DGL
{
	InputListener::InputListener()
	{
		m_KeyboardState.fill(IdleState);
		m_MouseButtonState.fill(IdleState);
	}

	void InputListener::Process(const System::WindowEvent& event)
	{
		event.Visit(
			[this](const System::WindowEvent::KeyPressed& data)
			{
				m_KeyboardState[static_cast<size_t>(data.Key)] = data.IsRepeated ? DownState : PressedState;
			},
			[this](const System::WindowEvent::KeyReleased& data)
			{
				m_KeyboardState[static_cast<size_t>(data.Key)] = ReleasedState;
			},
			[this](const System::WindowEvent::MouseButtonPressed& data)
			{
				m_MouseButtonState[static_cast<size_t>(data.Button)] = PressedState;
			},
			[this](const System::WindowEvent::MouseButtonReleased& data)
			{
				m_MouseButtonState[static_cast<size_t>(data.Button)] = ReleasedState;
			},
			[this](const System::WindowEvent::MouseMoved& data)
			{
				m_MousePosition = { data.MouseX, data.MouseY };
			},
			[](const auto&) {} // Ignore other events
		);
	}

	void InputListener::Update()
	{
		// Update keyboard state
		for (State& state : m_KeyboardState)
		{
			if (state.Pressed) { state = DownState; }
			else if (state.Released) { state = IdleState; }
		}

		// Update mouse button state
		for (State& state : m_MouseButtonState)
		{
			if (state.Pressed) { state = DownState; }
			else if (state.Released) { state = IdleState; }
		}
	}

	bool InputListener::IsKeyPressed(const Input::KeyboardKey key) const { return GetKeyState(key).Pressed; }
	bool InputListener::IsKeyDown(const Input::KeyboardKey key) const { return GetKeyState(key).Down; }
	bool InputListener::IsKeyReleased(const Input::KeyboardKey key) const { return GetKeyState(key).Released; }
	bool InputListener::IsMouseButtonPressed(const Input::MouseButton button) const { return GetMouseButtonState(button).Pressed; }
	bool InputListener::IsMouseButtonDown(const Input::MouseButton button) const { return GetMouseButtonState(button).Down; }
	bool InputListener::IsMouseButtonReleased(const Input::MouseButton button) const { return GetMouseButtonState(button).Released; }
	Int2 InputListener::GetMousePosition() const { return m_MousePosition; }

	InputListener::State InputListener::GetKeyState(const Input::KeyboardKey key) const
	{
		if (static_cast<int>(key) < Input::KeyboardKeyCount)
		{
			return m_KeyboardState[static_cast<size_t>(key)];
		}

		return IdleState;
	}

	InputListener::State InputListener::GetMouseButtonState(Input::MouseButton button) const
	{
		if (static_cast<int>(button) < Input::MouseButtonCount)
		{
			return m_MouseButtonState[static_cast<size_t>(button)];
		}

		return IdleState;
	}
}