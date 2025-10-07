module;

#include <memory>
#include <optional>
#include <format>

module DGL;

import System.Monitor;

namespace DGL
{
	WindowWrapper::WindowWrapper(std::shared_ptr<System::MonitorProvider> monitorProvider)
		: m_MonitorProvider(std::move(monitorProvider))
	{
	}

	Startup::StartupTask::Continuation WindowWrapper::Setup()
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

		// Create the window
		m_Window = System::CreateWindow({
			.Size = { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) },
			.Position = { windowLeft, windowTop },
			.Title = "DirectGL Application",
			.IsVisible = false,
			.OnError = [](const std::string_view error) { Error(std::string(error)); }
		});

		if (m_Window == nullptr)
		{
			Error("Failed to create window");
			return Abort;
		}

		Info("Window created successfully");
		return Continue;
	}

	void WindowWrapper::Teardown()
	{
		Info("Destroying Window");
		m_Window.reset();
	}

	std::optional<System::WindowEvent> WindowWrapper::PollEvent() { return m_Window->PollEvent(); }
	void WindowWrapper::SetPosition(const Math::Int2& position) { m_Window->SetPosition(position); }
	Math::Int2 WindowWrapper::GetPosition() const { return m_Window->GetPosition(); }
	void WindowWrapper::SetSize(const Math::Uint2& size) { m_Window->SetSize(size); }
	Math::Uint2 WindowWrapper::GetSize() const { return m_Window->GetSize(); }
	void WindowWrapper::SetTitle(const std::string_view title) { m_Window->SetTitle(title); }
	std::string WindowWrapper::GetTitle() const { return m_Window->GetTitle(); }
	void WindowWrapper::SetVisible(const bool visibility) { m_Window->SetVisible(visibility); }
	bool WindowWrapper::IsVisible() const { return m_Window->IsVisible(); }
	void WindowWrapper::SetResizable(const bool resizability) { m_Window->SetResizable(resizability); }
	bool WindowWrapper::IsResizable() const { return m_Window->IsResizable(); }
	bool WindowWrapper::RequestFocus() const { return m_Window->RequestFocus(); }
	System::NativeWindowHandle WindowWrapper::GetNativeHandle() const { return m_Window->GetNativeHandle(); }

}