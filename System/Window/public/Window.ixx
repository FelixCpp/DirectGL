// Project Name : Window
// File Name    : Window.ixx
// Author       : Felix Busch
// Created Date : 2025/10/01

export module System.Window;

#ifdef WINDOW_PLATFORM_WINDOWS
import :Win32Window;
#endif

export import :KeyboardKey;
export import :NativeWindowHandle;
export import :Window;
export import :WindowEvent;
