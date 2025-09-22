// Project Name : DirectGL
// File Name    : ConfigureWGLStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <Windows.h>
#include <functional>

export module DGL:ConfigureWGLStartupTask;

import Startup;

import :Window;

namespace DGL
{
	class ConfigureWGLStartupTask : public Startup::StartupTask
	{
	public:

		explicit ConfigureWGLStartupTask(const std::function<Window*()>& provider);
		Continuation Setup() override;
		void Teardown() override;

		void SwapBuffers();

	private:

		std::function<Window*()> m_WindowProvider;
		HDC m_DeviceContext;
		HGLRC m_RenderContext;

	};
}