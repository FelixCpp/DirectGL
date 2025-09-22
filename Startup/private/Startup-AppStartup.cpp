module;

#include <memory>

module Startup;

namespace Startup
{
	void AppStartup::AddStartupTask(const std::shared_ptr<StartupTask>& task)
	{
		m_Tasks.push_back(task);
	}

	void AppStartup::Run(const std::function<void()>& body)
	{
		size_t i = 0;

		// Setup all tasks in order
		for (; i < m_Tasks.size(); ++i)
		{
			if (m_Tasks[i]->Setup() == StartupTask::Abort)
			{
				break;
			}
		}

		// Only run the main body if all tasks were set up successfully
		if (i == m_Tasks.size())
		{
			body();
		}

		// Cleanup all tasks in reverse order
		for (size_t j = i; j-- > 0;)
		{
			m_Tasks[j]->Teardown();
		}
	}
}