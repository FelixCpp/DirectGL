// Project Name : Window
// File Name    : Window-WindowEvent.ixx
// Author       : Felix Busch
// Created Date : 2025/10/01

module;

#include <cstdint>
#include <variant>

export module System.Window:WindowEvent;

import DirectGL.Input;

namespace System
{
	/// This class is a container for window events.
	/// It encapsulates information about the type of event,
	/// as well as any relevant data associated with the event.
	///
	/// This allows for a unified way to handle various window events
	/// such as resizing, moving, focusing, and closing.
	export class WindowEvent
	{
	public:

		/// The user has requested to close the window.
		struct Closed {};

		/// The user has resized the window.
		struct Resized
		{
			uint32_t Width;
			uint32_t Height;
		};

		/// The user has entered a character.
		struct TextEntered
		{
			uint32_t Unicode;
		};

		/// The user has moved the mouse cursor.
		struct MouseMoved
		{
			int32_t MouseX;
			int32_t MouseY;
		};

		/// The mouse has entered the window client area.
		struct MouseEntered {};

		/// The mouse has left the window client area.
		struct MouseLeft {};

		/// The window has gained focus and is now the active window.
		struct FocusGained {};

		/// The window has lost focus and is no longer the active window.
		struct FocusLost {};

		struct KeyPressed
		{
			DGL::Input::KeyboardKey Key;
			bool IsAltDown;
			bool IsControlDown;
			bool IsShiftDown;
			bool IsSystemDown;
			bool IsRepeated;
		};

		struct KeyReleased
		{
			DGL::Input::KeyboardKey Key;
			bool IsAltDown;
			bool IsControlDown;
			bool IsShiftDown;
			bool IsSystemDown;
			bool IsRepeated;
		};

		struct MouseButtonPressed
		{
			DGL::Input::MouseButton Button;
			int32_t MouseX;
			int32_t MouseY;
		};

		struct MouseButtonReleased
		{
			DGL::Input::MouseButton Button;
			int32_t MouseX;
			int32_t MouseY;
		};

		/// Type definition of all possible window events.
		using EventType = std::variant<
			Closed,
			Resized,
			FocusGained,
			FocusLost,
			TextEntered,
			MouseMoved,
			MouseEntered,
			MouseLeft,
			KeyPressed,
			KeyReleased,
			MouseButtonPressed,
			MouseButtonReleased
		>;

		/// Construct a new WindowEvent object with a given event type.
		/// 
		/// @param type The type of the event.
		constexpr explicit WindowEvent(const EventType& type);

		template <typename ... Handlers>
		constexpr auto Visit(Handlers&& ... handlers) const;

		template <typename T> constexpr bool Is() const;

	private:

		EventType m_EventType;

	};
}

namespace System
{
	constexpr WindowEvent::WindowEvent(const EventType& type):
		m_EventType(type)
	{
	}

	template <typename ... T> struct MultiVisitor : T... { using T::operator()...; };

	template <typename ... Handlers>
	constexpr auto WindowEvent::Visit(Handlers&& ... handlers) const
	{
		return std::visit(MultiVisitor{ std::forward<Handlers>(handlers)... }, m_EventType);
	}

	template <typename T> constexpr bool WindowEvent::Is() const { return std::holds_alternative<T>(m_EventType); }

}