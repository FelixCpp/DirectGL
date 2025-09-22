module;

#include <glad/wgl.h>

#include <format>

module DGL;

static void wglDebugPreCallback(const char* name, GLADapiproc apiproc, int len_args, ...)
{
	DGL::Debug(std::format("Calling WGL function: {}", name));
}
static void wglDebugPostCallback(void* ret, const char* name, GLADapiproc apiproc, int len_args, ...)
{
	DGL::Debug(std::format("Called WGL function: {}", name));
}

namespace DGL
{
	PrepareWGLStartupTask::PrepareWGLStartupTask():
		m_LpszClassName("Dummy_WGL_Window"),
		m_Instance(GetModuleHandleA(nullptr)),
		m_DeviceContext(nullptr),
		m_RenderContext(nullptr),
		m_WindowHandle(nullptr)
	{
	}

	Startup::StartupTask::Continuation PrepareWGLStartupTask::Setup()
	{
		// Before we can initialize WGL properly, we need to create a "fake" window in order to load the WGL extensions.
		// After that, we can create the actual OpenGL rendering context for our real window.
		const WNDCLASSA wc = {
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = DefWindowProcA,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = m_Instance,
			.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = nullptr,
			.lpszMenuName = nullptr,
			.lpszClassName = m_LpszClassName,
		};

		if (not RegisterClassA(&wc))
		{
			Error("Failed to register window class for WGL initialization.");
			return Abort;
		}

		// Create the window
		m_WindowHandle = CreateWindowA(wc.lpszClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
		if (m_WindowHandle == nullptr)
		{
			Error("Failed to create window for WGL initialization.");
			return Abort;
		}

		// Get the device context
		m_DeviceContext = GetDC(m_WindowHandle);
		if (m_DeviceContext == nullptr)
		{
			Error("Failed to get the window's device context.");
			return Abort;
		}

		// Set some pixel format properties that we want for the window
		constexpr PIXELFORMATDESCRIPTOR desiredPixelFormatDescriptor =
		{
			.nSize = sizeof(PIXELFORMATDESCRIPTOR),
			.nVersion = 1,
			.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			.iPixelType = PFD_TYPE_RGBA,
			.cColorBits = 32,
			.cDepthBits = 24,
			.cStencilBits = 8,
			.iLayerType = PFD_MAIN_PLANE,
		};

		// Let the OS suggest a pixel format that matches our desired properties as closely as possible
		const int suggestedPixelFormat = ChoosePixelFormat(m_DeviceContext, &desiredPixelFormatDescriptor);
		if (suggestedPixelFormat == 0)
		{
			Error("Failed to choose a pixel format for the window's device context.");
			return Abort;
		}

		// Now we need to get the full details of the suggested pixel format
		PIXELFORMATDESCRIPTOR suggestedPixelFormatDescriptor;
		if (not DescribePixelFormat(m_DeviceContext, suggestedPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &suggestedPixelFormatDescriptor))
		{
			Error("Failed to describe the chosen pixel format.");
			return Abort;
		}

		// Set the pixel format for the device context
		if (not SetPixelFormat(m_DeviceContext, suggestedPixelFormat, &suggestedPixelFormatDescriptor))
		{
			Error("Failed to set the chosen pixel format for the window's device context.");
			return Abort;
		}

		// Create a temporary OpenGL rendering context
		m_RenderContext = wglCreateContext(m_DeviceContext);
		if (m_RenderContext == nullptr)
		{
			Error("Failed to create OpenGL rendering context.");
			return Abort;
		}

		// Activate the rendering context
		if (not wglMakeCurrent(m_DeviceContext, m_RenderContext))
		{
			Error("Failed to activate the OpenGL rendering context.");
			return Abort;
		}

		// If we reach this point, we have a valid OpenGL context and can load the WGL extensions
		if (not gladLoadWGL(m_DeviceContext, reinterpret_cast<GLADloadfunc>(&wglGetProcAddress)))
		{
			Error("Failed to initialize WGL.");
			return Abort;
		}

		return Continue;
	}

	void PrepareWGLStartupTask::Teardown()
	{
		if (not wglMakeCurrent(nullptr, nullptr))
		{
			Warning("Failed to release the OpenGL rendering context.");
		}

		if (m_RenderContext != nullptr)
		{
			if (not wglDeleteContext(m_RenderContext))
			{
				Error("Failed to delete the OpenGL rendering context.");
			}
		}

		if (m_DeviceContext != nullptr)
		{
			if (not ReleaseDC(m_WindowHandle, m_DeviceContext))
			{
				Error("Failed to release the window's device context.");
			}
		}

		if (m_WindowHandle != nullptr)
		{
			if (not DestroyWindow(m_WindowHandle))
			{
				Error("Failed to destroy the window.");
			}
		}

		if (not UnregisterClassA(m_LpszClassName, m_Instance))
		{
			Error("Failed to unregister the window class.");
		}
	}
}