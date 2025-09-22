// Project Name : DGL
// File Name    : DGL-WindowStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

module;

#include <memory>

export module DGL:WindowStartupTask;

import Startup;
import System.Monitor;

import :Window;

namespace DGL
{
	class WindowStartupTask : public Startup::StartupTask
	{
	public:

		explicit WindowStartupTask(
			std::unique_ptr<Window>& window,
			std::shared_ptr<System::MonitorProvider> monitorProvider
		);

		Continuation Setup() override;
		void Teardown() override;

	private:

		std::unique_ptr<Window>& m_Window;
		std::shared_ptr<System::MonitorProvider> m_MonitorProvider;

	};
}