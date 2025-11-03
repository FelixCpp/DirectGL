// Project Name : DirectGL-Startup
// File Name    : Startup-AppStartup.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

module;

#include <functional>
#include <memory>
#include <vector>

export module DirectGL.Startup:AppStartup;

import :StartupTask;

export namespace DGL
{
	class AppStartup
	{
	public:

		void AddStartupTask(const std::shared_ptr<StartupTask>& task);
		void Run(const std::function<void()>& body);

	private:

		std::vector<std::shared_ptr<StartupTask>> m_Tasks;

	};
}