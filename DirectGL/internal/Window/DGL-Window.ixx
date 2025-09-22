// Project Name : DGL
// File Name    : DGL-Window.ixx
// Author       : Felix Busch
// Created Date : 2025/09/16

module;

#include <Windows.h>

#include <string_view>
#include <variant>
#include <queue>
#include <string>
#include <optional>

export module DGL:Window;

import Math;

import :WindowEvent;

namespace DGL
{
	class Window
	{
	public:

		explicit Window(int windowLeft, int windowTop, int windowWidth, int windowHeght, std::string_view title);
		~Window();

		void SetVisible(bool visible);
		bool IsVisible() const;

		void SetSize(int width, int height);
		Math::Uint2 GetSize() const;

		void SetPosition(int x, int y);
		Math::Int2 GetPosition() const;

		void SetTitle(std::string_view title);
		std::string GetTitle() const;

		HWND GetNativeHandle() const;
		std::optional<WindowEvent> PollEvent();

		template <typename... Visitors>
		void HandleEvents(Visitors&& ... visitors)
		{
			while (const auto event = PollEvent())
			{
				event->Visit(std::forward<Visitors>(visitors)...);
			}
		}

	private:

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void QueueEvents() const;

		HWND m_WindowHandle;
		std::queue<WindowEvent> m_EventQueue;

		Math::Uint2 m_LastSize;
		bool m_IsResizing = false;

	};
}