// Project Name : DirectGL
// File Name    : ConfigureWGLStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <functional>
#include <memory>

export module DirectGL:ContextWrapper;

import DirectGL.Startup;
import DirectGL.Window;
import DirectGL.Context;

namespace DGL
{
	typedef std::function<Window* ()> WindowProvider;

	class ContextWrapper : public Context, public StartupTask
	{
	public:

		explicit ContextWrapper(const WindowProvider& windowProvider);

		Continuation Setup() override;
		void Teardown() override;

		void SetVerticalSyncEnabled(bool enabled) override;
		void SwapBuffers() override;

	private:

		WindowProvider m_WindowProvider;
		std::unique_ptr<Context> m_Context;

	};
}