// Project Name : DirectGL-Core
// File Name    : DirectGL-InputListener.ixx
// Author       : Felix Busch
// Created Date : 2025/10/09

module;

#include <array>

export module DirectGL:InputListener;

import DirectGL.Input;
import DirectGL.Math;

import System.Window;

namespace DGL
{
	class InputListener
	{
	private:

		struct State
		{
			bool Pressed;
			bool Down;
			bool Released;
		};

		static constexpr State PressedState = { .Pressed = true,  .Down = true,  .Released = false };
		static constexpr State DownState = { .Pressed = false, .Down = true,  .Released = false };
		static constexpr State ReleasedState = { .Pressed = false, .Down = false, .Released = true };
		static constexpr State IdleState = { .Pressed = false, .Down = false, .Released = false };

	public:

		InputListener();

		void Process(const System::WindowEvent& event);
		void Update();

		[[nodiscard]] bool IsKeyPressed(Input::KeyboardKey key) const;
		[[nodiscard]] bool IsKeyDown(Input::KeyboardKey key) const;
		[[nodiscard]] bool IsKeyReleased(Input::KeyboardKey key) const;

		[[nodiscard]] bool IsMouseButtonPressed(Input::MouseButton button) const;
		[[nodiscard]] bool IsMouseButtonDown(Input::MouseButton button) const;
		[[nodiscard]] bool IsMouseButtonReleased(Input::MouseButton button) const;

		[[nodiscard]] Math::Int2 GetMousePosition() const;

	private:

		[[nodiscard]] State GetKeyState(Input::KeyboardKey key) const;
		[[nodiscard]] State GetMouseButtonState(Input::MouseButton button) const;

		std::array<State, Input::KeyboardKeyCount> m_KeyboardState{};
		std::array<State, Input::MouseButtonCount> m_MouseButtonState{};

		Math::Int2 m_MousePosition;

	};
}