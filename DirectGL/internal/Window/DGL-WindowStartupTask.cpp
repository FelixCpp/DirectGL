module;

#include <memory>
#include <optional>
#include <format>

module DGL;

import System.Monitor;

namespace DGL
{
	WindowStartupTask::WindowStartupTask(
		std::unique_ptr<Window>& window,
		std::shared_ptr<System::MonitorProvider> monitorProvider
	) : m_Window(window), m_MonitorProvider(std::move(monitorProvider))
	{
	}

	Startup::StartupTask::Continuation WindowStartupTask::Setup()
	{
		int windowLeft = 100, windowTop = 100;

		constexpr int windowWidth = 900;
		constexpr int windowHeight = 900;

		// Try to center the window on the primary monitor on startup
		if (const auto primaryMonitor = m_MonitorProvider->GetPrimaryMonitor())
		{
			windowLeft = primaryMonitor->WorkArea.Left + (primaryMonitor->WorkArea.Width - windowWidth) / 2;
			windowTop = primaryMonitor->WorkArea.Top + (primaryMonitor->WorkArea.Height - windowHeight) / 2;
			Info(std::format("Initial Window Position: {}, {}", windowLeft, windowTop));
		}

		try
		{
			m_Window = std::make_unique<Window>(windowLeft, windowTop, windowWidth, windowHeight, "DGL Application");
		} catch (const std::exception& exception)
		{
			Error(std::format("Failed to create window: {}", exception.what()));
			return Abort;
		}

		Info("Window created successfully");
		return Continue;
	}

	void WindowStartupTask::Teardown()
	{
		Info("Destroying Window");
		m_Window.reset();
	}
}