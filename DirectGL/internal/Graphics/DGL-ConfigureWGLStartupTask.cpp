module;

#include <glad/wgl.h>
#include <format>
#include <array>

module DGL;

namespace DGL
{
	ConfigureWGLStartupTask::ConfigureWGLStartupTask(const std::function<Window* ()>& provider) :
		m_WindowProvider(provider),
		m_DeviceContext(nullptr),
		m_RenderContext(nullptr)
	{
	}

	Startup::StartupTask::Continuation ConfigureWGLStartupTask::Setup()
	{
		const HWND window = m_WindowProvider()->GetNativeHandle();
		if (window == nullptr)
		{
			Error("The provided window handle is null.");
			return Abort;
		}

		m_DeviceContext = GetDC(window);
		if (m_DeviceContext == nullptr)
		{
			Error("Failed to get the window's device context.");
			return Abort;
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
			0
		};

		int suggestedPixelFormatId = 0;
		UINT numFormats = 0;
		if (not wglChoosePixelFormatARB(m_DeviceContext, pixelFormatAttributes.data(), nullptr, 1, &suggestedPixelFormatId, &numFormats))
		{
			Error("Failed to choose a pixel format for the window's device context.");
			return Abort;
		}

		if (numFormats == 0)
		{
			Error("No suitable pixel format found for the window's device context.");
			return Abort;
		}

		PIXELFORMATDESCRIPTOR suggestedPixelFormat;
		if (not DescribePixelFormat(m_DeviceContext, suggestedPixelFormatId, sizeof(PIXELFORMATDESCRIPTOR), &suggestedPixelFormat))
		{
			Error("Failed to describe the chosen pixel format.");
			return Abort;
		}

		if (not SetPixelFormat(m_DeviceContext, suggestedPixelFormatId, &suggestedPixelFormat))
		{
			Error("Failed to set the chosen pixel format for the window's device context.");
			return Abort;
		}

		constexpr std::array contextAttributes = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
			0
		};

		m_RenderContext = wglCreateContextAttribsARB(m_DeviceContext, nullptr, contextAttributes.data());
		if (m_RenderContext == nullptr)
		{
			Error("Failed to create an OpenGL rendering context.");
			return Abort;
		}

		if (not wglMakeContextCurrentARB(m_DeviceContext, m_DeviceContext, m_RenderContext))
		{
			Error("Failed to activate the OpenGL rendering context.");
			return Abort;
		}

		if (not wglSwapIntervalEXT(1))
		{
			Error("Failed to set V-Sync on the OpenGL rendering context.");
			return Abort;
		}
		
		Info("Successfully created OpenGL rendering context.");
		return Continue;
	}

	void ConfigureWGLStartupTask::Teardown()
	{
		if (not wglMakeCurrent(nullptr, nullptr))
		{
			Warning("Couldn't release OpenGL rendering context.");
		}

		if (m_RenderContext != nullptr)
		{
			if (not wglDeleteContext(m_RenderContext))
			{
				Error("Couldn't delete OpenGL rendering context.");
			}
		}

		if (m_DeviceContext != nullptr)
		{
			const HWND window = m_WindowProvider()->GetNativeHandle();
			if (not ReleaseDC(window, m_DeviceContext))
			{
				Error("Couldn't release device context.");
			}
		}

		Info("Successfully released OpenGL rendering context and device context.");
	}

	void ConfigureWGLStartupTask::SwapBuffers()
	{
		if (m_DeviceContext != nullptr)
		{
			::SwapBuffers(m_DeviceContext);
		}
	}
}