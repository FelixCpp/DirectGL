// Project Name : DirectGL
// File Name    : DGL-PrepareWGLStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <Windows.h>

export module DGL:PrepareWGLStartupTask;

import Startup;

namespace DGL
{
	struct PrepareWGLStartupTask : Startup::StartupTask
	{
	public:

		PrepareWGLStartupTask();
		Continuation Setup() override;
		void Teardown() override;

	private:

		LPCSTR m_LpszClassName;
		HINSTANCE m_Instance;
		HDC m_DeviceContext;
		HGLRC m_RenderContext;
		HWND m_WindowHandle;

	};
}
