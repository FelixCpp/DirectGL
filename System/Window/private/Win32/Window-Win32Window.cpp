module;

#include <Windows.h>
#include <windowsx.h>

#include <stdexcept>
#include <string>
#include <memory>
#include <optional>
#include <bit>

module System.Window;

import DirectGL.Input;

namespace System
{
	[[nodiscard]] std::optional<std::wstring> StringToWide(const std::string_view source)
	{
		const int requiredLength = MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), nullptr, 0);
		if (requiredLength <= 0)
		{
			return std::nullopt;
		}

		std::wstring wideString(requiredLength, L'\0');
		const int convertedLength = MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), wideString.data(), requiredLength);
		if (convertedLength != requiredLength)
		{
			return std::nullopt;
		}

		return wideString;
	}

	[[nodiscard]] std::optional<std::string> WideToString(const std::wstring_view source)
	{
		const int requiredLength = WideCharToMultiByte(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
		if (requiredLength <= 0)
		{
			return std::nullopt;
		}

		std::string string(requiredLength, '\0');
		const int convertedLength = WideCharToMultiByte(CP_UTF8, 0, source.data(), static_cast<int>(source.length()), string.data(), requiredLength, nullptr, nullptr);
		if (convertedLength != requiredLength)
		{
			return std::nullopt;
		}

		return string;
	}

	[[nodiscard]] DGL::Input::KeyboardKey TranslateVirtualKey(const WPARAM key, const LPARAM flags)
	{
		switch (key)
		{
			// Check the scancode to distinguish between left and right shift
			case VK_SHIFT:
			{
				static const UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
				const UINT scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
				return scancode == lShift ? DGL::Input::KeyboardKey::LShift : DGL::Input::KeyboardKey::RShift;
			}

			// Check the "extended" flag to distinguish between left and right alt
			case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? DGL::Input::KeyboardKey::RAlt : DGL::Input::KeyboardKey::LAlt;

			// Check the "extended" flag to distinguish between left and right control
			case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? DGL::Input::KeyboardKey::RControl : DGL::Input::KeyboardKey::LControl;

			// Other keys are reported properly
			case VK_LWIN:       return DGL::Input::KeyboardKey::LSystem;
			case VK_RWIN:       return DGL::Input::KeyboardKey::RSystem;
			case VK_APPS:       return DGL::Input::KeyboardKey::Menu;
			case VK_OEM_1:      return DGL::Input::KeyboardKey::Semicolon;
			case VK_OEM_2:      return DGL::Input::KeyboardKey::Slash;
			case VK_OEM_PLUS:   return DGL::Input::KeyboardKey::Equal;
			case VK_OEM_MINUS:  return DGL::Input::KeyboardKey::Hyphen;
			case VK_OEM_4:      return DGL::Input::KeyboardKey::LBracket;
			case VK_OEM_6:      return DGL::Input::KeyboardKey::RBracket;
			case VK_OEM_COMMA:  return DGL::Input::KeyboardKey::Comma;
			case VK_OEM_PERIOD: return DGL::Input::KeyboardKey::Period;
			case VK_OEM_7:      return DGL::Input::KeyboardKey::Apostrophe;
			case VK_OEM_5:      return DGL::Input::KeyboardKey::Backslash;
			case VK_OEM_3:      return DGL::Input::KeyboardKey::Grave;
			case VK_ESCAPE:     return DGL::Input::KeyboardKey::Escape;
			case VK_SPACE:      return DGL::Input::KeyboardKey::Space;
			case VK_RETURN:     return DGL::Input::KeyboardKey::Enter;
			case VK_BACK:       return DGL::Input::KeyboardKey::Backspace;
			case VK_TAB:        return DGL::Input::KeyboardKey::Tab;
			case VK_PRIOR:      return DGL::Input::KeyboardKey::PageUp;
			case VK_NEXT:       return DGL::Input::KeyboardKey::PageDown;
			case VK_END:        return DGL::Input::KeyboardKey::End;
			case VK_HOME:       return DGL::Input::KeyboardKey::Home;
			case VK_INSERT:     return DGL::Input::KeyboardKey::Insert;
			case VK_DELETE:     return DGL::Input::KeyboardKey::Delete;
			case VK_ADD:        return DGL::Input::KeyboardKey::Add;
			case VK_SUBTRACT:   return DGL::Input::KeyboardKey::Subtract;
			case VK_MULTIPLY:   return DGL::Input::KeyboardKey::Multiply;
			case VK_DIVIDE:     return DGL::Input::KeyboardKey::Divide;
			case VK_PAUSE:      return DGL::Input::KeyboardKey::Pause;
			case VK_F1:         return DGL::Input::KeyboardKey::F1;
			case VK_F2:         return DGL::Input::KeyboardKey::F2;
			case VK_F3:         return DGL::Input::KeyboardKey::F3;
			case VK_F4:         return DGL::Input::KeyboardKey::F4;
			case VK_F5:         return DGL::Input::KeyboardKey::F5;
			case VK_F6:         return DGL::Input::KeyboardKey::F6;
			case VK_F7:         return DGL::Input::KeyboardKey::F7;
			case VK_F8:         return DGL::Input::KeyboardKey::F8;
			case VK_F9:         return DGL::Input::KeyboardKey::F9;
			case VK_F10:        return DGL::Input::KeyboardKey::F10;
			case VK_F11:        return DGL::Input::KeyboardKey::F11;
			case VK_F12:        return DGL::Input::KeyboardKey::F12;
			case VK_F13:        return DGL::Input::KeyboardKey::F13;
			case VK_F14:        return DGL::Input::KeyboardKey::F14;
			case VK_F15:        return DGL::Input::KeyboardKey::F15;
			case VK_LEFT:       return DGL::Input::KeyboardKey::Left;
			case VK_RIGHT:      return DGL::Input::KeyboardKey::Right;
			case VK_UP:         return DGL::Input::KeyboardKey::Up;
			case VK_DOWN:       return DGL::Input::KeyboardKey::Down;
			case VK_NUMPAD0:    return DGL::Input::KeyboardKey::Numpad0;
			case VK_NUMPAD1:    return DGL::Input::KeyboardKey::Numpad1;
			case VK_NUMPAD2:    return DGL::Input::KeyboardKey::Numpad2;
			case VK_NUMPAD3:    return DGL::Input::KeyboardKey::Numpad3;
			case VK_NUMPAD4:    return DGL::Input::KeyboardKey::Numpad4;
			case VK_NUMPAD5:    return DGL::Input::KeyboardKey::Numpad5;
			case VK_NUMPAD6:    return DGL::Input::KeyboardKey::Numpad6;
			case VK_NUMPAD7:    return DGL::Input::KeyboardKey::Numpad7;
			case VK_NUMPAD8:    return DGL::Input::KeyboardKey::Numpad8;
			case VK_NUMPAD9:    return DGL::Input::KeyboardKey::Numpad9;
			case 'A':           return DGL::Input::KeyboardKey::A;
			case 'Z':           return DGL::Input::KeyboardKey::Z;
			case 'E':           return DGL::Input::KeyboardKey::E;
			case 'R':           return DGL::Input::KeyboardKey::R;
			case 'T':           return DGL::Input::KeyboardKey::T;
			case 'Y':           return DGL::Input::KeyboardKey::Y;
			case 'U':           return DGL::Input::KeyboardKey::U;
			case 'I':           return DGL::Input::KeyboardKey::I;
			case 'O':           return DGL::Input::KeyboardKey::O;
			case 'P':           return DGL::Input::KeyboardKey::P;
			case 'Q':           return DGL::Input::KeyboardKey::Q;
			case 'S':           return DGL::Input::KeyboardKey::S;
			case 'D':           return DGL::Input::KeyboardKey::D;
			case 'F':           return DGL::Input::KeyboardKey::F;
			case 'G':           return DGL::Input::KeyboardKey::G;
			case 'H':           return DGL::Input::KeyboardKey::H;
			case 'J':           return DGL::Input::KeyboardKey::J;
			case 'K':           return DGL::Input::KeyboardKey::K;
			case 'L':           return DGL::Input::KeyboardKey::L;
			case 'M':           return DGL::Input::KeyboardKey::M;
			case 'W':           return DGL::Input::KeyboardKey::W;
			case 'X':           return DGL::Input::KeyboardKey::X;
			case 'C':           return DGL::Input::KeyboardKey::C;
			case 'V':           return DGL::Input::KeyboardKey::V;
			case 'B':           return DGL::Input::KeyboardKey::B;
			case 'N':           return DGL::Input::KeyboardKey::N;
			case '0':           return DGL::Input::KeyboardKey::Num0;
			case '1':           return DGL::Input::KeyboardKey::Num1;
			case '2':           return DGL::Input::KeyboardKey::Num2;
			case '3':           return DGL::Input::KeyboardKey::Num3;
			case '4':           return DGL::Input::KeyboardKey::Num4;
			case '5':           return DGL::Input::KeyboardKey::Num5;
			case '6':           return DGL::Input::KeyboardKey::Num6;
			case '7':           return DGL::Input::KeyboardKey::Num7;
			case '8':           return DGL::Input::KeyboardKey::Num8;
			case '9':           return DGL::Input::KeyboardKey::Num9;
			default:            return DGL::Input::KeyboardKey::Unknown;
		}
	}

	std::unique_ptr<Win32Window> Win32Window::Create(const CreateWindowProperties& properties)
	{
		try
		{
			return std::unique_ptr<Win32Window>(new Win32Window(properties));
		} catch (const std::exception& e)
		{
			if (properties.OnError)
			{
				properties.OnError(e.what());
			}

			return nullptr;
		}
	}

	Win32Window::~Win32Window()
	{
		{ // Deactivate mouse tracking
			TRACKMOUSEEVENT tme = {
				.cbSize = sizeof(TRACKMOUSEEVENT),
				.dwFlags = TME_CANCEL,
				.hwndTrack = m_Handle,
				.dwHoverTime = HOVER_DEFAULT
			};

			if (not TrackMouseEvent(&tme))
			{
				LogError("Couldn't track mouse leave event.");
			}
		}

		// No longer capture the cursor
		if (not ReleaseCapture())
		{
			LogError("Couldn't release mouse capture.");
		}

		if (m_Handle != nullptr)
		{
			if (not DestroyWindow(m_Handle))
			{
				LogError("Couldn't destroy window.");
			}
		}
	}

	std::optional<WindowEvent> Win32Window::PollEvent()
	{
		// If there are no events queued, ask the OS for new events
		if (m_EventQueue.empty())
		{
			QueueEvents();
		}

		if (not m_EventQueue.empty())
		{
			// Get the next event from the queue
			WindowEvent event = std::move(m_EventQueue.front());
			m_EventQueue.pop();
			return event;
		}

		// There are no events available
		return std::nullopt;
	}

	void Win32Window::SetPosition(const Math::Int2& size)
	{
		if (not SetWindowPos(m_Handle, nullptr, size.X, size.Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
		{
			LogError("Couldn't set window position.");
		}
	}

	Math::Int2 Win32Window::GetPosition() const
	{
		RECT rect;
		if (not GetWindowRect(m_Handle, &rect))
		{
			LogError("Couldn't retrieve window position.");
			return Math::Int2::Zero;
		}

		return Math::Int2{ rect.left, rect.top };
	}

	void Win32Window::SetSize(const Math::Uint2& size)
	{
		const DWORD dwStyle = GetWindowLongW(m_Handle, GWL_STYLE);
		const DWORD dwExStyle = GetWindowLongW(m_Handle, GWL_EXSTYLE);
		RECT rect = { .left = 0l, .top = 0l, .right = static_cast<LONG>(size.X), .bottom = static_cast<LONG>(size.Y) };
		if (not AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle))
		{
			LogError("Couldn't adjust window rectangle.");
			return;
		}

		const auto width = static_cast<int32_t>(rect.right - rect.left);
		const auto height = static_cast<int32_t>(rect.bottom - rect.top);
		SetWindowPos(m_Handle, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	}

	Math::Uint2 Win32Window::GetSize() const
	{
		RECT rect;
		if (not GetClientRect(m_Handle, &rect))
		{
			LogError("Couldn't retrieve window size.");
			return Math::Uint2::Zero;
		}

		return Math::Uint2{ static_cast<uint32_t>(rect.right - rect.left), static_cast<uint32_t>(rect.bottom - rect.top) };
	}

	void Win32Window::SetTitle(const std::string_view title)
	{
		const auto wideTitle = StringToWide(title);
		if (not wideTitle.has_value())
		{
			LogError("Failed to convert window title to wide string.");
			return;
		}

		if (not SetWindowTextW(m_Handle, wideTitle->c_str()))
		{
			LogError("Couldn't set window title.");
		}
	}

	std::string Win32Window::GetTitle() const
	{
		const int length = GetWindowTextLengthW(m_Handle);
		if (length < 0)
		{
			LogError("Couldn't retrieve window title length.");
			return {};
		}

		std::wstring wideTitle(length, L'\0');
		const int copiedLength = GetWindowTextW(m_Handle, wideTitle.data(), length + 1);
		if (copiedLength != length)
		{
			LogError("Couldn't retrieve window title.");
			return {};
		}

		const auto title = WideToString(wideTitle);
		if (not title.has_value())
		{
			LogError("Failed to convert window title from wide string.");
			return {};
		}

		return *title;
	}

	void Win32Window::SetVisible(const bool visibility)
	{
		ShowWindow(m_Handle, visibility ? SW_SHOW : SW_HIDE);
	}

	bool Win32Window::IsVisible() const
	{
		return IsWindowVisible(m_Handle) != FALSE;
	}

	void Win32Window::SetResizable(const bool resizability)
	{
		DWORD style = GetWindowLongW(m_Handle, GWL_STYLE);
		if (resizability)
		{
			style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}
		else
		{
			style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		}

		SetWindowLongW(m_Handle, GWL_STYLE, style);
	}

	bool Win32Window::IsResizable() const
	{
		const DWORD style = GetWindowLongW(m_Handle, GWL_STYLE);
		return (style & WS_THICKFRAME) != 0;
	}

	bool Win32Window::RequestFocus() const
	{
		// Check if the window is already in foreground
		if (GetForegroundWindow() == m_Handle)
		{
			return true; // Nothing to do
		}

		// Create a flash effect to get the user's attention
		FLASHWINFO flash = {
			.cbSize = sizeof(FLASHWINFO),
			.hwnd = m_Handle,
			.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG,
			.uCount = 3,
			.dwTimeout = 0
		};

		return FlashWindowEx(&flash) != FALSE;
	}

	NativeWindowHandle Win32Window::GetNativeHandle() const
	{
		return m_Handle;
	}

	Win32Window::Win32Window(const CreateWindowProperties& properties) :
		m_Handle(nullptr),
		m_OnError(properties.OnError),
		m_IsResizing(false),
		m_IsMouseInside(false),
		m_HighSurrogate(0)
	{
		// Define window class
		const WNDCLASSEXW wcex = {
			.cbSize = sizeof(WNDCLASSEXW),
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = &WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = GetModuleHandleW(nullptr),
			.hIcon = LoadIconW(nullptr, IDI_APPLICATION),
			.hCursor = LoadCursorW(nullptr, IDC_ARROW),
			.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
			.lpszMenuName = nullptr,
			.lpszClassName = L"CustomWindowClass",
			.hIconSm = LoadIconW(nullptr, IDI_APPLICATION)
		};

		// Note that we don't check for errors here, as RegisterClassExW
		// can fail if the class is already registered, which is not a problem.
		RegisterClassExW(&wcex);

		constexpr DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		constexpr DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

		RECT windowArea = { .left = 0l, .top = 0l, .right = static_cast<LONG>(properties.Size.X), .bottom = static_cast<LONG>(properties.Size.Y) };
		if (not AdjustWindowRectEx(&windowArea, dwStyle, FALSE, dwExStyle))
		{
			throw std::runtime_error("Failed to adjust window rectangle.");
		}

		const int realWindowWidth = static_cast<int>(windowArea.right - windowArea.left);
		const int realWindowHeight = static_cast<int>(windowArea.bottom - windowArea.top);

		const auto title = StringToWide(properties.Title);
		if (not title.has_value())
		{
			throw std::runtime_error("Failed to convert window title to wide string.");
		}

		m_Handle = CreateWindowExW(dwExStyle, wcex.lpszClassName, title->c_str(), dwStyle, properties.Position.X, properties.Position.Y, realWindowWidth, realWindowHeight, nullptr, nullptr, wcex.hInstance, this);
		if (m_Handle == nullptr)
		{
			throw std::runtime_error("Failed to create window.");
		}

		if (properties.IsVisible) ShowWindow(m_Handle, SW_SHOW);
	}

	void Win32Window::LogError(const std::string_view error) const
	{
		if (m_OnError)
		{
			m_OnError(error);
		}
	}

	void Win32Window::QueueEvents()
	{
		MSG msg = {};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	LRESULT Win32Window::WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
	{
		// Handle window creation to associate the Win32Window instance
		if (message == WM_CREATE)
		{
			const LPVOID params = std::bit_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams;
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(params));
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		// Retrieve the Win32Window instance associated with this window
		if (const auto window = std::bit_cast<Win32Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)))
		{
			if (window->m_Handle != nullptr)
			{
				window->HandleEvent(message, wParam, lParam);
			}
		}

		// Filter out messages that we don't want to be handled by the OS
		if (message == WM_CLOSE)
		{
			return 0;
		}

		// Return the default behavior to forward the handling of this message to the OS
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	void Win32Window::HandleEvent(const UINT message, const WPARAM wParam, const LPARAM lParam)
	{
		switch (message)
		{
			case WM_CLOSE: //< The user has requested to close the window
			{
				m_EventQueue.emplace(WindowEvent::Closed{});
			} break;

			case WM_MOUSEMOVE: //< The mouse has moved
			{
				// Capture the mouse if the user wants to drag it outside the window
				if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
				{
					// Only release the capture if we really have it
					if (GetCapture() == m_Handle)
					{
						ReleaseCapture();
					}
				} else if (GetCapture() != m_Handle)
				{
					// Set the capture to continue receiving mouse events outside the window
					SetCapture(m_Handle);
				}

				// Get the window client area to check if the mouse is inside
				RECT clientRect;
				if (not GetClientRect(m_Handle, &clientRect))
				{
					LogError("Couldn't retrieve client rectangle.");
					break;
				}

				// Extract the mouse position information
				const int32_t mouseX = GET_X_LPARAM(lParam);
				const int32_t mouseY = GET_Y_LPARAM(lParam);
				
				// Now check if the mouse is inside the client area
				if (mouseX >= clientRect.left and mouseX < clientRect.right and mouseY >= clientRect.top and mouseY < clientRect.bottom)
				{
					// Mouse is inside the client area
					if (not m_IsMouseInside)
					{
						m_IsMouseInside = true;

						TRACKMOUSEEVENT tme = {
							.cbSize = sizeof(TRACKMOUSEEVENT),
							.dwFlags = TME_LEAVE,
							.hwndTrack = m_Handle,
							.dwHoverTime = HOVER_DEFAULT
						};

						if (not TrackMouseEvent(&tme))
						{
							LogError("Couldn't track mouse leave event.");
						}

						m_EventQueue.emplace(WindowEvent::MouseEntered{});
					}
				}
				else
				{
					// Mouse is outside the client area
					if (m_IsMouseInside)
					{
						m_IsMouseInside = false;

						TRACKMOUSEEVENT tme = {
							.cbSize = sizeof(TRACKMOUSEEVENT),
							.dwFlags = TME_CANCEL,
							.hwndTrack = m_Handle,
							.dwHoverTime = HOVER_DEFAULT
						};

						if (not TrackMouseEvent(&tme))
						{
							LogError("Couldn't track mouse leave event.");
						}

						m_EventQueue.emplace(WindowEvent::MouseLeft{});
					}
				}

				m_EventQueue.emplace(WindowEvent::MouseMoved{ 
					.MouseX = mouseX,
					.MouseY = mouseY,
				});
			} break;

			// The user has entered a character
			case WM_CHAR:
			case WM_SYSCHAR:
			{
				// Check if the character is a high surrogate
				if (IS_HIGH_SURROGATE(wParam))
				{
					m_HighSurrogate = static_cast<WCHAR>(wParam);
					break;
				}

				// Check if the character is a low surrogate and we have a high surrogate stored
				if (IS_LOW_SURROGATE(wParam))
				{
					// Combine high and low surrogate to form a full codepoint
					if (m_HighSurrogate != 0)
					{
						uint32_t codepoint = 0;
						codepoint += (static_cast<uint32_t>(m_HighSurrogate) - 0xD800) << 10;
						codepoint += (static_cast<uint32_t>(wParam) - 0xDC00);
						codepoint += 0x10000;

						m_EventQueue.emplace(WindowEvent::TextEntered{ .Unicode = codepoint });
					}
				}
				else
				{
					// Not a surrogate pair, just use the character as is
					m_EventQueue.emplace(WindowEvent::TextEntered{ .Unicode = static_cast<uint32_t>(wParam) });
				}

				// Reset the high surrogate state to avoid incorrect combinations
				m_HighSurrogate = 0;
			} break;

			case WM_MOUSELEAVE: //< The mouse has left the window client area
			{
				if (m_IsMouseInside)
				{
					m_IsMouseInside = false;
					m_EventQueue.emplace(WindowEvent::MouseLeft{});
				}
			} break;

			case WM_SIZE:
			{
				// Ignore minimize events or if we're currently resizing
				const Math::Uint2 newSize = GetSize();

				if (wParam != SIZE_MINIMIZED and not m_IsResizing and m_LastSize != newSize)
				{
					m_LastSize = newSize;
					m_EventQueue.emplace(WindowEvent::Resized{
						.Width = newSize.X,
						.Height = newSize.Y
					});
				}
			} break;

			case WM_ENTERSIZEMOVE:
			{
				m_IsResizing = true;
			} break;

			case WM_EXITSIZEMOVE:
			{
				// We're only interested in resize operations
				// Early out if the size has not changed
				const Math::Uint2 newSize = GetSize();

				if (m_LastSize != newSize)
				{
					m_LastSize = newSize;
					m_EventQueue.emplace(WindowEvent::Resized{ 
						.Width = newSize.X,
						.Height = newSize.Y
					});
				}
			} break;

			case WM_SETFOCUS:
			{
				m_EventQueue.emplace(WindowEvent::FocusGained{});
			} break;

			case WM_KILLFOCUS:
			{
				m_EventQueue.emplace(WindowEvent::FocusLost{});
			} break;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				const DGL::Input::KeyboardKey key = TranslateVirtualKey(wParam, lParam);
				const bool isRepeated = (lParam & (1 << 30)) != 0;
				const bool isAltPressed = (lParam & (1 << 29)) != 0;
				const bool isCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				const bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				const bool isSuperPressed = (GetKeyState(VK_LWIN) & 0x8000) != 0 or (GetKeyState(VK_RWIN) & 0x8000) != 0;

				m_EventQueue.emplace(WindowEvent::KeyPressed{ 
					.Key = key,
					.IsAltDown = isAltPressed,
					.IsControlDown = isCtrlPressed,
					.IsShiftDown = isShiftPressed,
					.IsSystemDown = isSuperPressed,
					.IsRepeated = isRepeated
				});
			} break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				const DGL::Input::KeyboardKey key = TranslateVirtualKey(wParam, lParam);
				const bool isAltPressed = (lParam & (1 << 29)) != 0;
				const bool isCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				const bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				const bool isSuperPressed = (GetKeyState(VK_LWIN) & 0x8000) != 0 or (GetKeyState(VK_RWIN) & 0x8000) != 0;

				m_EventQueue.emplace(WindowEvent::KeyReleased{ 
					.Key = key,
					.IsAltDown = isAltPressed,
					.IsControlDown = isCtrlPressed,
					.IsShiftDown = isShiftPressed,
					.IsSystemDown = isSuperPressed
				});
			} break;

			case WM_LBUTTONDOWN:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonPressed{
					.Button = DGL::Input::MouseButton::Left,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_LBUTTONUP:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonReleased{
					.Button = DGL::Input::MouseButton::Left,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_RBUTTONDOWN:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonPressed{
					.Button = DGL::Input::MouseButton::Right,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_RBUTTONUP:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonReleased{
					.Button = DGL::Input::MouseButton::Right,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_MBUTTONDOWN:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonPressed{
					.Button = DGL::Input::MouseButton::Middle,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_MBUTTONUP:
			{
				m_EventQueue.emplace(WindowEvent::MouseButtonReleased{
					.Button = DGL::Input::MouseButton::Middle,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_XBUTTONDOWN:
			{
				const DGL::Input::MouseButton button = (HIWORD(wParam) & XBUTTON1) ? DGL::Input::MouseButton::Extra1 : DGL::Input::MouseButton::Extra2;
				m_EventQueue.emplace(WindowEvent::MouseButtonPressed{
					.Button = button,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;

			case WM_XBUTTONUP:
			{
				const DGL::Input::MouseButton button = (HIWORD(wParam) & XBUTTON1) ? DGL::Input::MouseButton::Extra1 : DGL::Input::MouseButton::Extra2;
				m_EventQueue.emplace(WindowEvent::MouseButtonReleased{
					.Button = button,
					.MouseX = GET_X_LPARAM(lParam),
					.MouseY = GET_Y_LPARAM(lParam),
				});
			} break;


			default: break;
		}
	}
}