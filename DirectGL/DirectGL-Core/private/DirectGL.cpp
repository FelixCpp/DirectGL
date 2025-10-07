module;

#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

module DGL;

import System.Monitor;

import Startup;
import LogForge;

import DirectGL.Renderer;

using namespace System;

/// <summary>
/// Sketch interface to be implemented by the user
/// </summary>
namespace DGL
{
	int Launch(const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		Library.LoggingChannel = std::make_unique<LoggingChannel>(std::make_unique<LogForge::DefaultLogger>(
			std::make_unique<LogForge::DevelopmentLogFilter>(),
			std::make_unique<LogForge::FmtLogPrinter>(),
			std::make_unique<LogForge::ConsoleLogOutput>()
		));

		Library.MonitorProvider = std::make_shared<MonitorProviderCache>(std::make_shared<Win32MonitorProvider>());
		Library.Context = std::make_unique<ContextWrapper>([] { return Library.Window.get(); });
		Library.Window = std::make_unique<WindowWrapper>(Library.MonitorProvider);

		Startup::AppStartup startup;
		startup.AddStartupTask(Library.LoggingChannel->GetStartupTask());
		startup.AddStartupTask(std::make_shared<ConfigureDPIStartupTask>());
		startup.AddStartupTask(Library.Window);
		startup.AddStartupTask(Library.Context);
		startup.AddStartupTask(std::make_shared<ConfigureGladStartupTask>());

		startup.Run([&factory]
		{
			Library.Sketch = factory();
			if (Library.Sketch == nullptr or not Library.Sketch->Setup())
			{
				Error("Couldn't setup the sketch");
				return;
			}

			Library.Window->SetVisible(true);

			const auto brush = Renderer::SolidColorBrush::Create(Color(255, 0, 0));
			const auto renderer = Renderer::VertexRenderer::Create(10'000);

			const Math::Matrix4x4 projection = Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(Library.Window->GetSize().X), static_cast<float>(Library.Window->GetSize().Y)), -1.0f, 1.0f);
			const Renderer::Vertices vertices = {
				// Positions
				{
					{ 100.0f, 100.0f, },
					{ 200.0f, 100.0f, },
					{ 200.0f, 200.0f, },
					{ 200.0f, 200.0f, },
					{ 100.0f, 200.0f, },
					{ 100.0f, 100.0f, },
				},
				// Indices
				{
					0, 1, 2,
					3, 4, 5,
				}
			};

			bool running = true;
			while (running)
			{
				while (const auto event = Library.Window->PollEvent())
				{
					const bool forwardToUser = event->Visit(
						[&running](const WindowEvent::Closed&)
						{
							Info("Window close event received");
							running = false;
							return true;
						},
						[&](const WindowEvent::Resized& resized)
						{
							Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
							return true;
						},
						[](const auto&)
						{
							return true;
						}
					);

					if (forwardToUser)
					{
						Library.Sketch->Event(*event);
					}
				}

				brush->UploadUniforms(projection);
				renderer->Render(vertices);

				Library.Sketch->Draw();
				Library.Context->Flush();
			}

			Library.Sketch->Destroy();
		});

		return 0;
	}
}

/// <summary>
/// Logging functionality
/// </summary>
namespace DGL
{
	void Log(const LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& time) { Library.LoggingChannel->Submit(level, message, time); }
	void Trace(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogLevel::Trace, message, time); }
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogLevel::Debug, message, time); }
	void Info(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogLevel::Info, message, time); }
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogLevel::Warning, message, time); }
	void Error(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogLevel::Error, message, time); }
}

/// <summary>
/// Windowing and event handling
/// </summary>
namespace DGL
{
	void ShowWindow() { Library.Window->SetVisible(true); }
	void HideWindow() { Library.Window->SetVisible(true); }
	bool IsWindowVisible() { return Library.Window->IsVisible(); }

	void SetWindowSize(const int width, const int height, const bool recenter)
	{
		Library.Window->SetSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

		if (recenter)
		{
			if (const auto monitor = Library.MonitorProvider->GetPrimaryMonitor())
			{
				const int windowLeft = monitor->WorkArea.Left + (monitor->WorkArea.Width - width) / 2;
				const int windowTop = monitor->WorkArea.Top + (monitor->WorkArea.Height - height) / 2;
				SetWindowPosition(windowLeft, windowTop);
			}
		}
	}

	Math::Uint2 GetWindowSize()	{ return Library.Window->GetSize(); }
	void SetWindowPosition(const int x, const int y) { Library.Window->SetPosition({ x, y }); }
	Math::Int2 GetWindowPosition() { return Library.Window->GetPosition(); }
	void SetWindowTitle(const std::string_view title) { Library.Window->SetTitle(title); }
	std::string GetWindowTitle() { return Library.Window->GetTitle(); }
}