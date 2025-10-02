// Project Name : Window
// File Name    : Window-Window.ixx
// Author       : Felix Busch
// Created Date : 2025/10/01

module;

#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <optional>

export module System.Window:Window;

import :NativeWindowHandle;
import :WindowEvent;

import Math;

namespace System
{
	/// This structure represents an interface for a window created and managed
	/// by the operating system.
	///
	/// Each platform will have its own implementation of this interface,
	/// providing the necessary functionality to create, display, and manage
	/// windows in a platform-specific manner.
	///
	/// Note that it is not possible to create an instance of this structure
	export struct Window
	{
		virtual ~Window() = default;

		/// Get a pending event from the window's event queue.
		/// 
		/// @return The next event if available, std::nullopt if no events
		///			are pending.
		virtual std::optional<WindowEvent> PollEvent() = 0;

		/// Set the position of the window on the screen.
		/// The coordinates are given in pixels, with
		/// (0, 0) being the top-left corner of the screen.
		///
		/// @param position The new position of the window.
		virtual void SetPosition(const Math::Int2& position) = 0;

		/// Get the current position of the window on the screen.
		/// The coordinates are given in pixels, with
		/// (0, 0) being the top-left corner of the screen.
		///
		/// @return The current position of the window.
		virtual Math::Int2 GetPosition() const = 0;

		/// Set the size of the window's client area.
		/// The size is given in pixels, excluding window decorations
		/// such as title bars and borders.
		/// 
		/// @param size The new size of the window's client area.
		virtual void SetSize(const Math::Uint2& size) = 0;

		/// Get the current size of the window's client area.
		/// The size is given in pixels, excluding window decorations
		/// such as title bars and borders.
		///
		/// @return The current size of the window's client area.
		virtual Math::Uint2 GetSize() const = 0;

		/// Set the title of the window displayed in the title bar.
		/// 
		/// @param title The new title for the window.
		virtual void SetTitle(std::string_view title) = 0;

		/// Get the current title of the window displayed in the title bar.
		///
		/// @return The current title of the window.
		virtual std::string GetTitle() const = 0;

		/// Set the visibility of the window.
		///
		/// @param visibility True to make the window visible, false to hide it.
		virtual void SetVisible(bool visibility) = 0;

		/// Get the current visibility state of the window.
		/// 
		/// @return True if the window is visible, false if it is hidden.
		virtual bool IsVisible() const = 0;

		/// Set whether the window is resizable by the user.
		/// 
		/// @param resizability True to make the window resizable, false
		///						to make it fixed size.
		virtual void SetResizable(bool resizability) = 0;

		/// Get whether the window is resizable by the user.
		/// 
		/// @return True if the window is resizable, false if it
		///			is fixed size.
		virtual bool IsResizable() const = 0;

		/// Request focus for the window, bringing it to the foreground
		/// and making it the active window.
		///
		/// @return True if the focus request was successful, false otherwise.
		virtual bool RequestFocus() const = 0;

		/// Get the native handle of the window allowing for platform-specific
		/// operations.
		/// 
		/// @return The native window handle.
		virtual NativeWindowHandle GetNativeHandle() const = 0;

	};

	/// Type definition for the error callback function.
	typedef std::function<void(std::string_view)> OnErrorCallback;

	/// This structure contains properties for creating a new window.
	export struct CreateWindowProperties
	{
		Math::Uint2 Size;
		Math::Int2 Position;
		std::string_view Title;
		bool IsVisible;
		OnErrorCallback OnError;
	};

	export std::unique_ptr<Window> CreateWindow(const CreateWindowProperties& properties);
}