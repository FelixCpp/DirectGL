module;

#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include <variant>

module DGL;

import System.Monitor;

import Startup;

using namespace System;

/// <summary>
/// Sketch interface to be implemented by the user
/// </summary>
namespace DGL
{
	[[nodiscard]] std::unique_ptr<ResourceFactory> CreateResourceFactory(const GraphicsAPI api)
	{
		switch (api)
		{
			case GraphicsAPI::OpenGL: return std::make_unique<OpenGLResourceFactory>();
			default: throw std::invalid_argument("Couldn't create ResourceFactory for unknown GraphicsAPI.");
		}
	}

	int Launch(GraphicsAPI api, const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		Library.LoggingChannel = std::make_unique<LoggingChannel>(std::make_unique<Logger>(
			std::make_unique<DevelopmentLogFilter>(),
			std::make_unique<FmtLogPrinter>(),
			std::make_unique<ConsoleLogOutput>()
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

		startup.Run([api, &factory]
		{
			Library.ResourceFactory = CreateResourceFactory(api);
			Library.Renderer = Library.ResourceFactory->CreateRenderer(10'000);
			Library.RenderTarget = Library.ResourceFactory->CreateWindowRenderTarget(*Library.Window, *Library.Renderer);

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
							Info("Window close event received");
							running = false;
							return true;
						},
						[&](const WindowEvent::Resized& resized)
						{
							Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
							Library.RenderTarget->Resize(resized.Width, resized.Height);
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

				Library.RenderTarget->Begin();
				Library.Sketch->Draw();
				Library.RenderTarget->End();
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

/// <summary>
/// Graphics and rendering
/// </summary>
namespace DGL
{
	std::unique_ptr<OffscreenRenderTarget> CreateOffscreenRenderTarget(const uint32_t width, const uint32_t height) { return Library.ResourceFactory->CreateFramebuffer(width, height, *Library.Renderer); }
	std::unique_ptr<Shader> CreateShader(const std::string_view shaderSource, const ShaderType type) { return Library.ResourceFactory->CreateShader(shaderSource, type); }
	std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader) { return Library.ResourceFactory->CreateShaderProgram(vertexShader, fragmentShader); }
	std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath) { return Library.ResourceFactory->CreateTexture(filepath); }
	std::unique_ptr<TextureSampler> CreateTextureSampler(const TextureWrapMode wrapMode, const TextureFilterMode filterMode) { return Library.ResourceFactory->CreateTextureSampler(wrapMode, filterMode); }
	std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(const Color color) { return Library.ResourceFactory->CreateSolidColorBrush(color); }
}

namespace DGL
{
	Renderer& GetRenderer() { return *Library.Renderer; }
	RenderTarget& GetRenderTarget() { return *Library.RenderTarget; }
}
