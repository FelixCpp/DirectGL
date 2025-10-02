// Project Name : Window
// File Name    : Window-NativeWindowHandle.ixx
// Author       : Felix Busch
// Created Date : 2025/10/01

module;

#ifdef WINDOW_PLATFORM_WINDOWS
#include <Windows.h>
#endif

export module System.Window:NativeWindowHandle;

namespace System
{
#ifdef WINDOW_PLATFORM_WINDOWS
	/// This type represents a native window handle on Windows platforms.
	/// Note that this code is being only compiled on Windows platforms.
	export typedef HWND NativeWindowHandle;
#else
	export typedef void* NativeWindowHandle;
#endif
}