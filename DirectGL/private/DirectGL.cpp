module;

#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

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
			using enum GraphicsAPI;
			case OpenGL: return std::make_unique<OpenGLResourceFactory>();
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

		Library.WGL = std::make_shared<ConfigureWGLStartupTask>([] { return Library.Window.get(); });
		Library.MonitorProvider = std::make_shared<MonitorProviderCache>(std::make_shared<Win32MonitorProvider>());
		
		Startup::AppStartup startup;
		startup.AddStartupTask(Library.LoggingChannel->GetStartupTask());
		startup.AddStartupTask(std::make_shared<ConfigureDPIStartupTask>());
		startup.AddStartupTask(std::make_shared<PrepareWGLStartupTask>());
		startup.AddStartupTask(std::make_shared<WindowStartupTask>(Library.Window, Library.MonitorProvider));
		startup.AddStartupTask(Library.WGL);
		startup.AddStartupTask(std::make_shared<ConfigureGladStartupTask>());

		startup.Run([api, &factory]
		{
			Camera camera({ 900.0f, 900.0f });

			Library.ResourceFactory = CreateResourceFactory(api);
			Library.Renderer = Library.ResourceFactory->CreateRenderer(10'000);
			Library.RenderTarget = std::make_unique<RenderTarget>(*Library.Renderer, &camera);

			const auto sampler = CreateTextureSampler();
			const auto texture = CreateTexture("test.jpg");
			const auto brush = CreateTextureBrush(*texture, *sampler);

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
						[&camera](const WindowEvent::Resized& resized)
						{
							camera.SetSize({ static_cast<float>(resized.Width), static_cast<float>(resized.Height) });
							return true;
						},
						[](const WindowEvent::MouseMoved& move)
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

				Library.RenderTarget->FillRoundedRectangle(
					Math::FloatBoundary::FromLTRB(100.0f, 100.0f, 300.0f, 300.0f),
					BorderRadius::All(Radius::Elliptical(60.0f, 60.0f)),
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
	std::unique_ptr<Shader> CreateShader(const std::string_view shaderSource, const ShaderType type) { return Library.ResourceFactory->CreateShader(shaderSource, type); }
	std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader) { return Library.ResourceFactory->CreateShaderProgram(vertexShader, fragmentShader); }
	std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath) { return Library.ResourceFactory->CreateTexture(filepath); }
	std::unique_ptr<TextureSampler> CreateTextureSampler() { return Library.ResourceFactory->CreateTextureSampler(); }
	std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(const Color& color) { return Library.ResourceFactory->CreateSolidColorBrush(color); }
	std::unique_ptr<TextureBrush> CreateTextureBrush(const Texture& texture, const TextureSampler& sampler) { return Library.ResourceFactory->CreateTextureBrush(texture, sampler); }
}

namespace DGL
{
	Renderer& GetRenderer() { return *Library.Renderer; }
	RenderTarget& GetRenderTarget() { return *Library.RenderTarget; }
}
