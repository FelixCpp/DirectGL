// Project Name : Window
// File Name    : Window-Win32Window.ixx
// Author       : Felix Busch
// Created Date : 2025/10/01

module;

#include <Windows.h>

#include <queue>

export module System.Window:Win32Window;

import :Window;
import :WindowEvent;

namespace System
{
	class Win32Window : public Window
	{
	public:

		/// Create a new window with the specified properties.
		///
		/// @param properties Startup properties for the window.
		/// 
		/// @return A unique pointer to the created window.
		static std::unique_ptr<Win32Window> Create(const CreateWindowProperties& properties);

		/// Cleanup resources associated with the window.
		~Win32Window() override;

		/// Get a pending event from the window's event queue.
		/// 
		/// @return The next event if available, std::nullopt if no events
		///			are pending.
		std::optional<WindowEvent> PollEvent() override;

		/// Set the position of the window on the screen.
		/// The coordinates are given in pixels, with
		/// (0, 0) being the top-left corner of the screen.
		///
		/// @param size The new position of the window.
		void SetPosition(const Math::Int2& size) override;

		/// Get the current position of the window on the screen.
		/// The coordinates are given in pixels, with
		/// (0, 0) being the top-left corner of the screen.
		///
		/// @return The current position of the window.
		Math::Int2 GetPosition() const override;

		/// Set the size of the window's client area.
		/// The size is given in pixels, excluding window decorations
		/// such as title bars and borders.
		/// 
		/// @param size The new size of the window's client area.
		void SetSize(const Math::Uint2& size) override;

		/// Get the current size of the window's client area.
		/// The size is given in pixels, excluding window decorations
		/// such as title bars and borders.
		///
		/// @return The current size of the window's client area.
		Math::Uint2 GetSize() const override;

		/// Set the title of the window displayed in the title bar.
		/// 
		/// @param title The new title for the window.
		void SetTitle(std::string_view title) override;

		/// Get the current title of the window displayed in the title bar.
		///
		/// @return The current title of the window.
		std::string GetTitle() const override;

		/// Set the visibility of the window.
		///
		/// @param visibility True to make the window visible, false to hide it.
		void SetVisible(bool visibility) override;

		/// Get the current visibility state of the window.
		/// 
		/// @return True if the window is visible, false if it is hidden.
		bool IsVisible() const override;

		/// Set whether the window is resizable by the user.
		/// 
		/// @param resizability True to make the window resizable, false
		///						to make it fixed size.
		void SetResizable(bool resizability) override;

		/// Get whether the window is resizable by the user.
		/// 
		/// @return True if the window is resizable, false if it
		///			is fixed size.
		bool IsResizable() const override;

		/// Request focus for the window, bringing it to the foreground
		/// and making it the active window.
		///
		/// @return True if the focus request was successful, false otherwise.
		bool RequestFocus() const override;

		/// Get the native handle of the window allowing for platform-specific
		/// operations.
		/// 
		/// @return The native window handle.
		NativeWindowHandle GetNativeHandle() const override;

	private:

		/// Construct a Win32Window from an existing native window handle.
		/// @param properties Properties defining the window behavior.
		explicit Win32Window(const CreateWindowProperties& properties);

		/// Log an error message
		///
		/// @param error The error message to log.
		void LogError(std::string_view error) const;

		/// Poll for window events and add them to the event queue.
		static void QueueEvents();

		/// Custom window procedure to handle window messages.
		/// 
		/// @param hWnd The window handle firing the message.
		/// @param message The message id being fired.
		/// @param wParam Additional message information.
		/// @param lParam Additional message information.
		/// 
		/// @return Behavior depends on the message being processed.
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		/// Callback to handle a specific window message.
		void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam);

		NativeWindowHandle m_Handle;
		OnErrorCallback m_OnError;
		std::queue<WindowEvent> m_EventQueue;

		bool m_IsResizing;
		bool m_IsMouseInside;

		WCHAR m_HighSurrogate;
		Math::Uint2 m_LastSize;

	};
}