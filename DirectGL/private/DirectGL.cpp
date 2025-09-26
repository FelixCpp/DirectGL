module;

#include <memory>
#include <string>
#include <string_view>

module DGL;

import System.Monitor;

import Startup;

using namespace System;

/// <summary>
/// Sketch interface to be implemented by the user
/// </summary>
namespace DGL
{
	int Launch(const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		Library.LoggingChannel = std::make_unique<LoggingChannel>(std::make_unique<Logger>(
			std::make_unique<DevelopmentLogFilter>(),
			std::make_unique<FmtLogPrinter>(),
			std::make_unique<ConsoleLogOutput>()
		));

		Library.WGL = std::make_shared<ConfigureWGLStartupTask>([] { return Library.Window.get(); });
		Library.MonitorProvider = std::make_shared<MonitorProviderCache>(std::make_shared<Win32MonitorProvider>());
		
		Startup::AppStartup startup;
		startup.AddStartupTask(Library.LoggingChannel->GetStartupTask());
		startup.AddStartupTask(std::make_shared<ConfigureDPIStartupTask>());
		startup.AddStartupTask(std::make_shared<PrepareWGLStartupTask>());
		startup.AddStartupTask(std::make_shared<WindowStartupTask>(Library.Window, Library.MonitorProvider));
		startup.AddStartupTask(Library.WGL);
		startup.AddStartupTask(std::make_shared<ConfigureGladStartupTask>());

		startup.Run([&]
		{
			Library.Renderer = std::make_unique<OpenGLRenderer>();
			Library.RenderTarget = std::make_unique<RenderTarget>(*Library.Renderer);

			const auto brush = RadialGradientBrush::Create({
				.GradientStops = {
					GradientStop {.Position = 0.0f, .Color = Color(255, 0, 0) },
					GradientStop {.Position = 0.3f, .Color = Color(0, 255, 0) },
					GradientStop {.Position = 0.5f, .Color = Color(0, 0, 255) },
					GradientStop {.Position = 1.0f, .Color = Color(255, 255, 0) },
				},
				.ExtendMode = ExtendMode::Mirror,
				.GammaMode = GammaMode::Gamma1_0,
				.Center = { 0.5f, 0.5f, },
				.Offset = { 0.0f, 0.0f, },
				.Radius = { 0.5f, 0.25f, },
			});

			Library.Sketch = factory();
			if (Library.Sketch == nullptr or not Library.Sketch->Setup())
			{
				Error("Couldn't setup the sketch");
				return;
			}

			Library.Window->SetVisible(true);

			bool running = true;
			while (running)
			{
				while (const auto event = Library.Window->PollEvent())
				{
					const bool forwardToUser = event->Visit(
						[&running](const WindowEvent::Closed&)
						{
							running = false;
							return true;
						},
						[](const WindowEvent::Resized& resized)
						{
							
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

				/*Library.RenderTarget->FillRoundedRectangle(
					Math::FloatBoundary::FromLTWH(100.0f, 100.0f, 400.0f, 400.0f),
					BorderRadius::All(Radius::Elliptical(30.0f, 15.0f)),
					*brush
				);*/

				Library.RenderTarget->FillEllipse(
					Math::Float2(100.0f, 100.0f),
					Radius::Circular(30.0f),
					*brush
				);

				Library.Sketch->Draw();
				Library.WGL->SwapBuffers();
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
		Library.Window->SetSize(width, height);

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
	void SetWindowPosition(const int x, const int y) { Library.Window->SetPosition(x, y); }
	Math::Int2 GetWindowPosition() { return Library.Window->GetPosition(); }
	void SetWindowTitle(const std::string_view title) { Library.Window->SetTitle(title); }
	std::string GetWindowTitle() { return Library.Window->GetTitle(); }
}

/// <summary>
/// Graphics and rendering
/// </summary>
namespace DGL
{
	
}
