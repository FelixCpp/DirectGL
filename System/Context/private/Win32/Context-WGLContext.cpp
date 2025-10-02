module;

#include <glad/wgl.h>

#include <memory>
#include <bit>
#include <array>
#include <vector>
#include <string_view>

module System.Context;

namespace System
{
	struct WindowHandleDeleter { static void operator()(const HWND window) { DestroyWindow(window); } };
	struct RenderContextDeleter { static void operator()(const HGLRC context) { wglDeleteContext(context); } };

	struct DeviceContextDeleter
	{
		HWND AssociatedWindowHandle;
		void operator()(const HDC deviceContext) const { ReleaseDC(AssociatedWindowHandle, deviceContext); }
	};

	using WindowHandle = std::unique_ptr<std::remove_pointer_t<HWND>, WindowHandleDeleter>;
	using RenderingContextHandle = std::unique_ptr<std::remove_pointer_t<HGLRC>, RenderContextDeleter>;
	using DeviceContextHandle = std::unique_ptr<std::remove_pointer_t<HDC>, DeviceContextDeleter>;

	inline bool PrepareWGLFunctions(const std::function<void(std::string_view)>& logError)
	{
		// Before we can load WGL functions, we need a temporary context.
		// This is a bit of a chicken-and-egg problem, but we can solve it by creating a dummy window.
		const WindowHandle dummyWindow(CreateWindowA("STATIC", "Dummy", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr));
		if (dummyWindow == nullptr)
		{
			logError("Failed to create dummy window for OpenGL context creation.");
			return false;
		}

		// Get the device context
		const DeviceContextHandle deviceContext(GetDC(dummyWindow.get()), DeviceContextDeleter{ .AssociatedWindowHandle = dummyWindow.get() });
		if (deviceContext == nullptr)
		{
			logError("Failed to get device context from dummy window.");
			return false;
		}

		// Set a pixel format
		PIXELFORMATDESCRIPTOR pfd = {
			.nSize = sizeof(PIXELFORMATDESCRIPTOR),
			.nVersion = 1,
			.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			.iPixelType = PFD_TYPE_RGBA,
			.cColorBits = 32,
			.cDepthBits = 24,
			.cStencilBits = 8,
			.iLayerType = PFD_MAIN_PLANE,
		};

		// Choose and set the pixel format
		const int suggestedPixelFormat = ChoosePixelFormat(deviceContext.get(), &pfd);
		if (suggestedPixelFormat == 0)
		{
			logError("Failed to choose pixel format for OpenGL context.");
			return false;
		}

		// Describe the pixel format to ensure it matches our requirements
		if (not DescribePixelFormat(deviceContext.get(), suggestedPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
		{
			logError("Failed to describe pixel format for OpenGL context.");
			return false;
		}

		// Activate the pixel format
		if (not SetPixelFormat(deviceContext.get(), suggestedPixelFormat, &pfd))
		{
			logError("Failed to set pixel format for OpenGL context.");
			return false;
		}

		// Create a temporary context
		const RenderingContextHandle tempContext(wglCreateContext(deviceContext.get()));
		if (tempContext == nullptr)
		{
			logError("Failed to create temporary OpenGL context.");
			return false;
		}

		// Make the temporary context current
		if (not wglMakeCurrent(deviceContext.get(), tempContext.get()))
		{
			logError("Failed to activate temporary OpenGL context.");
			return false;
		}

		// Now we can load WGL functions
		if (not gladLoadWGL(deviceContext.get(), std::bit_cast<GLADloadfunc>(&wglGetProcAddress)))
		{
			wglMakeCurrent(nullptr, nullptr);
			return false;
		}

		wglMakeCurrent(nullptr, nullptr);
		return true;
	}

	std::unique_ptr<WGLContext> WGLContext::Create(const ContextSettings& settings)
	{
		// Lambda to log errors if a callback is provided
		const auto logError = [&settings](const std::string_view message)
		{
			if (settings.OnError)
			{
				settings.OnError(message);
			}
		};

		// Ensure WGL functions are loaded & ready to use
		if (not PrepareWGLFunctions(logError))
		{
			logError("Failed to prepare WGL functions.");
			return nullptr;
		}

		const HWND windowHandle = settings.ParentWindow;
		if (windowHandle == nullptr)
		{
			logError("Invalid parent window handle provided for OpenGL context creation.");
			return nullptr;
		}

		// At this point we can create a real OpenGL context
		const HDC deviceContext = GetDC(windowHandle);
		if (deviceContext == nullptr)
		{
			logError("Failed to get device context from parent window.");
			return nullptr;
		}

		constexpr std::array pixelFormatAttributes = {
			WGL_DRAW_TO_WINDOW_ARB, TRUE,
			WGL_SUPPORT_OPENGL_ARB, TRUE,
			WGL_DOUBLE_BUFFER_ARB, TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_SAMPLES_ARB, 4,
			0
		};

		int suggestedPixelFormatId = 0;
		UINT numFormats = 0;
		if (not wglChoosePixelFormatARB(deviceContext, pixelFormatAttributes.data(), nullptr, 1, &suggestedPixelFormatId, &numFormats))
		{
			logError("Failed to choose a pixel format for the window's device context.");
			ReleaseDC(windowHandle, deviceContext);
			return nullptr;
		}

		if (numFormats == 0)
		{
			logError("No suitable pixel format found for the window's device context.");
			return nullptr;
		}

		PIXELFORMATDESCRIPTOR suggestedPixelFormat;
		if (not DescribePixelFormat(deviceContext, suggestedPixelFormatId, sizeof(PIXELFORMATDESCRIPTOR), &suggestedPixelFormat))
		{
			logError("Failed to describe the chosen pixel format.");
			ReleaseDC(windowHandle, deviceContext);
			return nullptr;
		}

		if (not SetPixelFormat(deviceContext, suggestedPixelFormatId, &suggestedPixelFormat))
		{
			logError("Failed to set the chosen pixel format for the window's device context.");
			ReleaseDC(windowHandle, deviceContext);
			return nullptr;
		}

		std::vector<int> contextAttributes;
		contextAttributes.append_range(std::array{ WGL_CONTEXT_MAJOR_VERSION_ARB, settings.MajorVersion });
		contextAttributes.append_range(std::array{ WGL_CONTEXT_MINOR_VERSION_ARB, settings.MinorVersion });
		contextAttributes.append_range(std::array{ WGL_CONTEXT_PROFILE_MASK_ARB, settings.IsCompatibilityContext ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB });
		if (settings.IsDebuggingContext) { contextAttributes.append_range(std::array{ WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB }); }
		contextAttributes.push_back(0); // Null-terminate the attributes list

		const HGLRC renderContext = wglCreateContextAttribsARB(deviceContext, nullptr, contextAttributes.data());
		if (renderContext == nullptr)
		{
			logError("Failed to create an OpenGL rendering context.");
			ReleaseDC(windowHandle, deviceContext);
			return nullptr;
		}

		if (not wglMakeContextCurrentARB(deviceContext, deviceContext, renderContext))
		{
			logError("Failed to activate the OpenGL rendering context.");
			wglDeleteContext(renderContext);
			ReleaseDC(windowHandle, deviceContext);
			return nullptr;
		}

		return std::unique_ptr<WGLContext>(new WGLContext(deviceContext, renderContext, windowHandle, settings.OnError));
	}

	WGLContext::~WGLContext()
	{
		if (m_RenderingContext)
		{
			// Only release the context if it's the current one
			const HGLRC currentContext = wglGetCurrentContext();
			if (currentContext == m_RenderingContext)
			{
				if (not wglMakeCurrent(nullptr, nullptr))
				{
					LogError("Failed to release the current OpenGL rendering context.");
				}
			}

			// Delete the rendering context
			if (not wglDeleteContext(m_RenderingContext))
			{
				LogError("Failed to delete the OpenGL rendering context.");
			}
		}

		if (m_DeviceContext)
		{
			// Delete the device context
			if (not ReleaseDC(m_ParentHandle, m_DeviceContext))
			{
				LogError("Failed to release the device context.");
			}
		}
	}


	void WGLContext::SetVerticalSyncEnabled(const bool enabled)
	{
		if (not wglSwapIntervalEXT(enabled ? 1 : 0))
		{
			LogError("Failed to set V-Sync on the OpenGL rendering context.");
		}
	}

	void WGLContext::Flush()
	{
		SwapBuffers(m_DeviceContext);
	}

	WGLContext::WGLContext(const HDC deviceContext, const HGLRC renderingContext, const NativeWindowHandle parentHandle, const std::function<void(std::string_view)>& onError):
		m_DeviceContext(deviceContext),
		m_RenderingContext(renderingContext),
		m_ParentHandle(parentHandle),
		m_OnError(onError)
	{
	}

	void WGLContext::LogError(const std::string_view message) const
	{
		if (m_OnError)
		{
			m_OnError(message);
		}
	}
}