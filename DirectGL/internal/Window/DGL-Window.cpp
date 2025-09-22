module;

#include <Windows.h>
#include <windowsx.h>

#include <string>
#include <optional>
#include <memory>

module DGL;

namespace DGL
{
	[[nodiscard]] std::wstring StringToWide(const std::string_view source)
	{
		const int requiredLength = MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), nullptr, 0);
		Check(requiredLength > 0, [] { return "Couldn't convert string to wide string."; });

		std::wstring wideString(requiredLength, L'\0');
		const int convertedLength = MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), wideString.data(), requiredLength);
		Check(convertedLength == requiredLength, [] { return "Couldn't convert string to wide string."; });

		return wideString;
	}

	[[nodiscard]] std::string WideToString(const std::wstring_view source)
	{
		const int requiredLength = WideCharToMultiByte(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
		Check(requiredLength > 0, [] { return "Couldn't convert wide string to string."; });

		std::string string(requiredLength, '\0');
		const int convertedLength = WideCharToMultiByte(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), string.data(), requiredLength, nullptr, nullptr);
		Check(convertedLength == requiredLength, [] { return "Couldn't convert wide string to string."; });

		return string;
	}

	Window::Window(const int windowLeft, const int windowTop, const int windowWidth, const int windowHeght, const std::string_view title)
	{
		constexpr DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		constexpr DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

		RECT windowArea = { .left = 0l, .top = 0l, .right = static_cast<LONG>(windowWidth), .bottom = static_cast<LONG>(windowHeght) };
		Check(AdjustWindowRectEx(&windowArea, dwStyle, FALSE, dwExStyle), [] { return "Couldn't adjust window rectangle."; });

		const int realWindowWidth = static_cast<int>(windowArea.right - windowArea.left);
		const int realWindowHeight = static_cast<int>(windowArea.bottom - windowArea.top);

		const WNDCLASSEXW wcex = {
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = &WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = GetModuleHandle(nullptr),
			.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = nullptr,
			.lpszMenuName = nullptr,
			.lpszClassName = TEXT("SpikyWindowClass"),
			.hIconSm = LoadIcon(nullptr, IDI_APPLICATION)
		};

		Check(RegisterClassEx(&wcex), [] { return "Couldn't register window class."; });

		m_WindowHandle = CheckNotNull(
			CreateWindowExW(dwExStyle, wcex.lpszClassName, StringToWide(title).c_str(), dwStyle, windowLeft, windowTop, realWindowWidth, realWindowHeight, nullptr, nullptr, wcex.hInstance, this),
			[] { return "Couldn't create window."; }
		);
	}

	Window::~Window()
	{
		DestroyWindow(m_WindowHandle);
	}

	void Window::SetVisible(const bool visible)
	{
		ShowWindow(m_WindowHandle, visible ? SW_SHOW : SW_HIDE);
	}

	bool Window::IsVisible() const
	{
		return IsWindowVisible(m_WindowHandle) != FALSE;
	}

	void Window::SetSize(const int width, const int height)
	{
		const DWORD dwStyle = GetWindowLongW(m_WindowHandle, GWL_STYLE);
		const DWORD dwExStyle = GetWindowLongW(m_WindowHandle, GWL_EXSTYLE);

		RECT windowArea = { .left = 0l, .top = 0l, .right = static_cast<LONG>(width), .bottom = static_cast<LONG>(height) };
		Check(AdjustWindowRectEx(&windowArea, dwStyle, FALSE, dwExStyle), [] { return "Couldn't adjust window rectangle."; });

		const int windowWidth = static_cast<int>(windowArea.right - windowArea.left);
		const int windowHeight = static_cast<int>(windowArea.bottom - windowArea.top);

		SetWindowPos(m_WindowHandle, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
	}

	Math::Uint2 Window::GetSize() const
	{
		RECT windowArea = {};
		Check(GetClientRect(m_WindowHandle, &windowArea), [] { return "Couldn't retrieve window size"; });

		const uint32_t width = static_cast<uint32_t>(windowArea.right - windowArea.left);
		const uint32_t height = static_cast<uint32_t>(windowArea.bottom - windowArea.top);
		return { width, height };
	}


	void Window::SetPosition(const int x, const int y)
	{
		SetWindowPos(m_WindowHandle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	Math::Int2 Window::GetPosition() const
	{
		RECT windowBoundary = {};
		Check(GetWindowRect(m_WindowHandle, &windowBoundary), [] { return "Couldn't retrieve window position."; }	);

		const auto x = static_cast<int32_t>(windowBoundary.left);
		const auto y = static_cast<int32_t>(windowBoundary.top);
		return { x, y };
	}

	void Window::SetTitle(const std::string_view title)
	{
		SetWindowTextW(m_WindowHandle, StringToWide(title).c_str());
	}

	std::string Window::GetTitle() const
	{
		const int length = GetWindowTextLengthW(m_WindowHandle);
		Check(length >= 0, [] { return "Couldn't retrieve window title length."; });

		std::wstring wideTitle(length, L'\0');
		const int retrievedLength = GetWindowTextW(m_WindowHandle, wideTitle.data(), length + 1);
		Check(retrievedLength == length, [] { return "Couldn't retrieve window title."; });

		return WideToString(wideTitle);
	}

	HWND Window::GetNativeHandle() const
	{
		return m_WindowHandle;
	}

	std::optional<WindowEvent> Window::PollEvent()
	{
		if (m_EventQueue.empty())
		{
			QueueEvents();
		}

		if (not m_EventQueue.empty())
		{
			WindowEvent event = std::move(m_EventQueue.front());
			m_EventQueue.pop();
			return event;
		}

		return std::nullopt;
	}


	LRESULT Window::WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams));
			return 1;
		}

		if (const auto window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)); window && window->m_WindowHandle)
		{
			return window->HandleMessage(uMsg, wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	LRESULT Window::HandleMessage(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_CLOSE:
			{
				m_EventQueue.emplace(WindowEvent::Closed{});
			} break;

			case WM_ENTERSIZEMOVE:
			{
				m_IsResizing = true;
			} break;

			case WM_SIZE:
			{
				if (wParam != SIZE_MINIMIZED and not m_IsResizing and m_LastSize != GetSize())
				{
					m_LastSize = GetSize();
					m_EventQueue.emplace(WindowEvent::Resized{ .Width = m_LastSize.X, .Height = m_LastSize.Y });
				}
			} break;

			case WM_EXITSIZEMOVE:
			{
				m_IsResizing = false;

				// Ignore cases where the size didn't actually change (window move without resize)
				if (m_LastSize != GetSize())
				{
					m_LastSize = GetSize();
					m_EventQueue.emplace(WindowEvent::Resized{ .Width = m_LastSize.X, .Height = m_LastSize.Y });
				}
			} break;

			case WM_LBUTTONDOWN:
			{
				const int32_t mouseX = GET_X_LPARAM(lParam);
				const int32_t mouseY = GET_Y_LPARAM(lParam);
				m_EventQueue.emplace(WindowEvent::MousePressed{ .MouseX = mouseX, .MouseY = mouseY });
			} break;

			default:
				return DefWindowProcA(m_WindowHandle, uMsg, wParam, lParam);
		}

		return 0;
	}

	void Window::QueueEvents() const
	{
		MSG msg = {};
		while (PeekMessageW(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

}
