// Project Name : DirectGL
// File Name    : ConfigureWGLStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <Windows.h>
#include <functional>
#include <memory>

export module DGL:ContextWrapper;

import Startup;
import System.Window;
import System.Context;

namespace DGL
{
	typedef std::function<System::Window* ()> WindowProvider;

	class ContextWrapper : public System::Context, public Startup::StartupTask
	{
	public:

		explicit ContextWrapper(const WindowProvider& windowProvider);

		Continuation Setup() override;
		void Teardown() override;

		void SetVerticalSyncEnabled(bool enabled) override;
		void Flush() override;

	private:

		WindowProvider m_WindowProvider;
		std::unique_ptr<Context> m_Context;

	};
}