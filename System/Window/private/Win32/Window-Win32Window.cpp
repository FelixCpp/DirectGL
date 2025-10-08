module;

#include <Windows.h>
#include <windowsx.h>

#include <stdexcept>
#include <string>
#include <memory>
#include <optional>
#include <bit>

module System.Window;

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

	[[nodiscard]] KeyboardKey TranslateVirtualKey(const WPARAM key, const LPARAM flags)
	{
		switch (key)
		{
			// Check the scancode to distinguish between left and right shift
			case VK_SHIFT:
			{
				static const UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
				const UINT scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
				return scancode == lShift ? KeyboardKey::LShift : KeyboardKey::RShift;
			}

			// Check the "extended" flag to distinguish between left and right alt
			case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? KeyboardKey::RAlt : KeyboardKey::LAlt;

			// Check the "extended" flag to distinguish between left and right control
			case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? KeyboardKey::RControl : KeyboardKey::LControl;

			// Other keys are reported properly
			case VK_LWIN:       return KeyboardKey::LSystem;
			case VK_RWIN:       return KeyboardKey::RSystem;
			case VK_APPS:       return KeyboardKey::Menu;
			case VK_OEM_1:      return KeyboardKey::Semicolon;
			case VK_OEM_2:      return KeyboardKey::Slash;
			case VK_OEM_PLUS:   return KeyboardKey::Equal;
			case VK_OEM_MINUS:  return KeyboardKey::Hyphen;
			case VK_OEM_4:      return KeyboardKey::LBracket;
			case VK_OEM_6:      return KeyboardKey::RBracket;
			case VK_OEM_COMMA:  return KeyboardKey::Comma;
			case VK_OEM_PERIOD: return KeyboardKey::Period;
			case VK_OEM_7:      return KeyboardKey::Apostrophe;
			case VK_OEM_5:      return KeyboardKey::Backslash;
			case VK_OEM_3:      return KeyboardKey::Grave;
			case VK_ESCAPE:     return KeyboardKey::Escape;
			case VK_SPACE:      return KeyboardKey::Space;
			case VK_RETURN:     return KeyboardKey::Enter;
			case VK_BACK:       return KeyboardKey::Backspace;
			case VK_TAB:        return KeyboardKey::Tab;
			case VK_PRIOR:      return KeyboardKey::PageUp;
			case VK_NEXT:       return KeyboardKey::PageDown;
			case VK_END:        return KeyboardKey::End;
			case VK_HOME:       return KeyboardKey::Home;
			case VK_INSERT:     return KeyboardKey::Insert;
			case VK_DELETE:     return KeyboardKey::Delete;
			case VK_ADD:        return KeyboardKey::Add;
			case VK_SUBTRACT:   return KeyboardKey::Subtract;
			case VK_MULTIPLY:   return KeyboardKey::Multiply;
			case VK_DIVIDE:     return KeyboardKey::Divide;
			case VK_PAUSE:      return KeyboardKey::Pause;
			case VK_F1:         return KeyboardKey::F1;
			case VK_F2:         return KeyboardKey::F2;
			case VK_F3:         return KeyboardKey::F3;
			case VK_F4:         return KeyboardKey::F4;
			case VK_F5:         return KeyboardKey::F5;
			case VK_F6:         return KeyboardKey::F6;
			case VK_F7:         return KeyboardKey::F7;
			case VK_F8:         return KeyboardKey::F8;
			case VK_F9:         return KeyboardKey::F9;
			case VK_F10:        return KeyboardKey::F10;
			case VK_F11:        return KeyboardKey::F11;
			case VK_F12:        return KeyboardKey::F12;
			case VK_F13:        return KeyboardKey::F13;
			case VK_F14:        return KeyboardKey::F14;
			case VK_F15:        return KeyboardKey::F15;
			case VK_LEFT:       return KeyboardKey::Left;
			case VK_RIGHT:      return KeyboardKey::Right;
			case VK_UP:         return KeyboardKey::Up;
			case VK_DOWN:       return KeyboardKey::Down;
			case VK_NUMPAD0:    return KeyboardKey::Numpad0;
			case VK_NUMPAD1:    return KeyboardKey::Numpad1;
			case VK_NUMPAD2:    return KeyboardKey::Numpad2;
			case VK_NUMPAD3:    return KeyboardKey::Numpad3;
			case VK_NUMPAD4:    return KeyboardKey::Numpad4;
			case VK_NUMPAD5:    return KeyboardKey::Numpad5;
			case VK_NUMPAD6:    return KeyboardKey::Numpad6;
			case VK_NUMPAD7:    return KeyboardKey::Numpad7;
			case VK_NUMPAD8:    return KeyboardKey::Numpad8;
			case VK_NUMPAD9:    return KeyboardKey::Numpad9;
			case 'A':           return KeyboardKey::A;
			case 'Z':           return KeyboardKey::Z;
			case 'E':           return KeyboardKey::E;
			case 'R':           return KeyboardKey::R;
			case 'T':           return KeyboardKey::T;
			case 'Y':           return KeyboardKey::Y;
			case 'U':           return KeyboardKey::U;
			case 'I':           return KeyboardKey::I;
			case 'O':           return KeyboardKey::O;
			case 'P':           return KeyboardKey::P;
			case 'Q':           return KeyboardKey::Q;
			case 'S':           return KeyboardKey::S;
			case 'D':           return KeyboardKey::D;
			case 'F':           return KeyboardKey::F;
			case 'G':           return KeyboardKey::G;
			case 'H':           return KeyboardKey::H;
			case 'J':           return KeyboardKey::J;
			case 'K':           return KeyboardKey::K;
			case 'L':           return KeyboardKey::L;
			case 'M':           return KeyboardKey::M;
			case 'W':           return KeyboardKey::W;
			case 'X':           return KeyboardKey::X;
			case 'C':           return KeyboardKey::C;
			case 'V':           return KeyboardKey::V;
			case 'B':           return KeyboardKey::B;
			case 'N':           return KeyboardKey::N;
			case '0':           return KeyboardKey::Num0;
			case '1':           return KeyboardKey::Num1;
			case '2':           return KeyboardKey::Num2;
			case '3':           return KeyboardKey::Num3;
			case '4':           return KeyboardKey::Num4;
			case '5':           return KeyboardKey::Num5;
			case '6':           return KeyboardKey::Num6;
			case '7':           return KeyboardKey::Num7;
			case '8':           return KeyboardKey::Num8;
			case '9':           return KeyboardKey::Num9;
			default:            return KeyboardKey::Unknown;
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
				const KeyboardKey key = TranslateVirtualKey(wParam, lParam);
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
				const KeyboardKey key = TranslateVirtualKey(wParam, lParam);
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

			default: break;
		}
	}
}