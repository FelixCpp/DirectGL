module;

#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include <glad/gl.h>

module DirectGL;

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
	void LaunchImpl(const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		Library.Logger = std::make_unique<Logging::AsyncLogger>(std::make_unique<LogForge::DefaultLogger>(
			std::make_unique<LogForge::DevelopmentLogFilter>(),
			std::make_unique<LogForge::FmtLogPrinter>(),
			std::make_unique<LogForge::ConsoleLogOutput>()
		));

		Library.MonitorProvider = std::make_shared<MonitorProviderCache>(std::make_shared<Win32MonitorProvider>());
		Library.Context = std::make_unique<ContextWrapper>([] { return Library.Window.get(); });
		Library.Window = std::make_unique<WindowWrapper>(Library.MonitorProvider);

		Startup::AppStartup startup;
		startup.AddStartupTask(Library.Logger);
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

			Library.RenderStateStack.Clear();

			Library.SolidFillBrush = Renderer::SolidColorBrush::Create(Color(255, 255, 255));
			Library.SolidStrokeBrush = Renderer::SolidColorBrush::Create(Color(255, 255, 255));
			Library.VertexRenderer = Renderer::VertexRenderer::Create(10'000);

			while (not Library.CloseRequested)
			{
				while (const auto event = Library.Window->PollEvent())
				{
					const bool forwardToUser = event->Visit(
						[](const WindowEvent::Closed&)
						{
							Info("Window close event received");
							Quit();
							return true;
						},
						[&](const WindowEvent::Resized& resized)
						{
							glViewport(0, 0, static_cast<GLsizei>(resized.Width), static_cast<GLsizei>(resized.Height));
							Info(std::format("Window has been resized: {}, {}", resized.Width, resized.Height));
							return true;
						},
						[&](const WindowEvent::KeyReleased& keyPressed)
						{
							if (keyPressed.Key == KeyboardKey::R and keyPressed.IsControlDown)
							{
								Info("Restart requested via Ctrl+R");
								Restart();
							}
							else if (keyPressed.Key == KeyboardKey::Q and keyPressed.IsControlDown)
							{
								Info("Quit requested via Ctrl+Q");
								Quit();
							}

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

				Library.Sketch->Draw();
				Library.Context->Flush();
			}

			Library.Sketch->Destroy();
		});
	}

	int Launch(const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		ExitType exitType;
		int exitCode;

		do
		{
			// Reset the library state
			Library = {};

			// Run the application
			LaunchImpl(factory);

			exitType = Library.ExitType;
			exitCode = Library.ExitCode;
		} while (exitType == ExitType::Restart);

		return exitCode;
	}
}

/// <summary>
/// Logging functionality
/// </summary>
namespace DGL
{
	void Log(const LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Log(level, message, time); }
	void Trace(const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Trace(message, time); }
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Debug(message, time); }
	void Info(const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Info(message, time); }
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Warning(message, time); }
	void Error(const std::string& message, const std::chrono::system_clock::time_point& time) { Logging::Error(message, time); }
}

namespace DGL
{
	void Restart() { Library.ExitType = ExitType::Restart; Library.CloseRequested = true; }
	void Restart(const int exitCode) { SetExitCode(exitCode); Restart(); }
	void Quit() { Library.ExitType = ExitType::Quit; Library.CloseRequested = true; }
	void Quit(const int exitCode) { SetExitCode(exitCode); Quit(); }
	void SetExitCode(const int exitCode) { Library.ExitCode = exitCode; }
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

namespace DGL
{
	void PushState() { Library.RenderStateStack.PushState(); }
	void PopState() { Library.RenderStateStack.PopState(); }
	RenderState& PeekState() { return Library.RenderStateStack.PeekState(); }
	void Fill(const Color color) { auto& state = PeekState(); state.FillColor = color; state.IsFillEnabled = true; }
	void NoFill() { PeekState().IsFillEnabled = false; }
	void Stroke(const Color color) { auto& state = PeekState(); state.StrokeColor = color; state.IsStrokeEnabled = true; }
	void NoStroke() { PeekState().IsStrokeEnabled = false; }
	void StrokeWeight(const float strokeWeight) { PeekState().StrokeWeight = strokeWeight; }
	void Blend(const BlendMode& blendMode) { PeekState().BlendMode = blendMode; }

	void Background(const Color color)
	{
		// Get the window size
		const auto [windowWidth, windowHeight] = Library.Window->GetSize();
		const auto windowArea = Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		const auto camera = Math::Matrix4x4::Orthographic(windowArea, -1.0f, 1.0f);

		// Paint a rectangle covering the entire window with the specified color
		const auto vertices = Renderer::GetFilledRectangle(windowArea);

		Library.SolidFillBrush->SetColor(color);
		Library.SolidFillBrush->UploadUniforms(camera);
		Library.VertexRenderer->Render(vertices, BlendModes::Opaque);
	}

	void Ellipse(const float x, const float y, const float width, const float height)
	{
		// Create an ellipse centered at (x, y) with the specified width and height
		const auto radius = Math::Radius::Elliptical(width * 0.5f, height * 0.5f);
		const auto center = Math::Float2{ x, y };

		// Get the current render state
		const auto& state = PeekState();

		const auto [windowWidth, windowHeight] = Library.Window->GetSize();
		const auto camera = Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)), -1.0f, 1.0f);

		// Render the geometry using the fill shape if fill is enabled
		if (state.IsFillEnabled)
		{
			const auto vertices = Renderer::GetFilledEllipse(center, radius, 64);

			Library.SolidFillBrush->SetColor(state.FillColor);
			Library.SolidFillBrush->UploadUniforms(camera);
			Library.VertexRenderer->Render(vertices, state.BlendMode);
		}

		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			const auto vertices = Renderer::GetOutlinedEllipse(center, radius, state.StrokeWeight, 64);
		
			Library.SolidStrokeBrush->SetColor(state.StrokeColor);
			Library.SolidStrokeBrush->UploadUniforms(camera);
			Library.VertexRenderer->Render(vertices, state.BlendMode);
		}
	}

	void Circle(const float x, const float y, const float diameter)
	{
		Ellipse(x, y, diameter, diameter);
	}

	void Line(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			const auto [windowWidth, windowHeight] = Library.Window->GetSize();
			const auto camera = Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)), -1.0f, 1.0f);
			const auto vertices = Renderer::GetLine(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, state.StrokeWeight);

			Library.SolidStrokeBrush->SetColor(state.StrokeColor);
			Library.SolidStrokeBrush->UploadUniforms(camera);
			Library.VertexRenderer->Render(vertices, state.BlendMode);
		}
	}

	void Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		// Get the current render state
		const auto& state = PeekState();
		const auto [windowWidth, windowHeight] = Library.Window->GetSize();
		const auto camera = Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)), -1.0f, 1.0f);

		// Render the geometry using the fill shape if fill is enabled
		if (state.IsFillEnabled)
		{
			// Create a triangle with the specified vertices
			const auto v1 = Math::Float2{ x1, y1 };
			const auto v2 = Math::Float2{ x2, y2 };
			const auto v3 = Math::Float2{ x3, y3 };

			const auto vertices = Renderer::GetFilledTriangle(v1, v2, v3);
			Library.SolidFillBrush->SetColor(state.FillColor);
			Library.SolidFillBrush->UploadUniforms(camera);
			Library.VertexRenderer->Render(vertices, state.BlendMode);
		}

		// TODO(Felix): Implement stroke for triangles
	}
}
