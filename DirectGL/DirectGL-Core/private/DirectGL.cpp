module;

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include <Glad/gl.h>

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
			const auto defaultActivator = std::make_unique<Blending::DefaultBlendModeActivator>();
			Library.BlendModeActivator = std::make_unique<Blending::CachingBlendModeActivator>(*defaultActivator);
			Library.ShapeFactory = std::make_unique<ShapeRenderer::ShapeFactory>();
			Library.ShapeRenderer = ShapeRenderer::ShapeRenderer::Create(10'000, 10'000);
			Library.TextureRenderer = TextureRenderer::TextureRenderer::Create(500);
			Library.DepthProvider = std::make_unique<IncrementalDepthProvider>(0.0f, 1.0f / 20'000.f);

			Library.RendererFacade = std::make_unique<RendererFacade>(
				*Library.TextureRenderer,
				*Library.ShapeRenderer,
				*Library.ShapeFactory,
				*Library.DepthProvider
			);

			Library.MainGraphicsLayer = MainGraphicsLayer::Create(
				Library.Window->GetSize(),
				*Library.RendererFacade,
				*Library.ShapeFactory,
				*Library.BlendModeActivator
			);

			Library.GraphicsLayerStack = std::make_unique<GraphicsLayerStack>(Library.MainGraphicsLayer.get());

			Library.Sketch = factory();
			if (Library.Sketch == nullptr or not Library.Sketch->Setup())
			{
				Error("Couldn't setup the sketch");
				return;
			}

			Library.Window->SetVisible(true);

			std::chrono::duration<float> deltaTime{ 0.0f };
			auto lastFrameTime = std::chrono::high_resolution_clock::now();
			while (not Library.CloseRequested)
			{
				// Before we process events, we need to update the input listener
				Library.InputListener.Update();

				// Poll all events from the window
				while (const auto event = Library.Window->PollEvent())
				{
					event->Visit(
						[](const WindowEvent::Closed&)
						{
							Info("Window close event received");
							Quit();
						},
						[&](const WindowEvent::Resized& resizeEvent)
						{
							Library.MainGraphicsLayer->Resize({ resizeEvent.Width, resizeEvent.Height });
							Redraw(); //!< Request a redraw after the window has been resized.
							Info(std::format("Window has been resized: {}, {}", resizeEvent.Width, resizeEvent.Height));
						},
						[&](const WindowEvent::KeyReleased& keyEvent)
						{
							if (keyEvent.Key == KeyboardKey::F5)
							{
								Info("Restart requested via F5");
								Restart();
							}
							else if (keyEvent.Key == KeyboardKey::Escape)
							{
								Info("Quit requested via Escape");
								Quit();
							}
						},
						[](const auto&) {}
					);

					// Forward the event to the input listener as well as the sketch
					Library.InputListener.Process(*event);
					Library.Sketch->Event(*event);
				}

				// Let the user render the next frame
				if (not Library.IsPaused or Library.FrameCount == 0 or Library.UserRequestedRedraw)
				{
					// Note that this needs to happen before we call the Draw function
					Library.UserRequestedRedraw = false;

					Library.DepthProvider->ResetDepth();
					Library.MainGraphicsLayer->BeginDraw();
					Library.Sketch->Draw(deltaTime.count());
					Library.MainGraphicsLayer->EndDraw();

					// Present the rendered frame on screen
					Library.Context->Flush();

					const auto now = std::chrono::high_resolution_clock::now();
					deltaTime = now - lastFrameTime;
					lastFrameTime = now;
				}

				// Increment the number of frames processed
				++Library.FrameCount;
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
	bool IsKeyPressed(const KeyboardKey key) { return Library.InputListener.IsKeyPressed(key); }
	bool IsKeyDown(const KeyboardKey key) { return Library.InputListener.IsKeyDown(key); }
	bool IsKeyReleased(const KeyboardKey key) { return Library.InputListener.IsKeyReleased(key); }
	bool IsMouseButtonPressed(const MouseButton button) { return Library.InputListener.IsMouseButtonPressed(button); }
	bool IsMouseButtonDown(const MouseButton button) { return Library.InputListener.IsMouseButtonDown(button); }
	bool IsMouseButtonReleased(const MouseButton button) { return Library.InputListener.IsMouseButtonReleased(button); }
	Math::Int2 GetMousePosition() { return Library.InputListener.GetMousePosition(); }
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
	/*Renderer::Renderer& GetDefaultRenderer() { return *Library.VertexRenderer; }
	Renderer::ShapeFactory& GetDefaultShapeFactory() { return *Library.ShapeFactory; }
	GraphicsLayer& GetMainGraphicsLayer() { return *Library.MainGraphicsLayer; }*/
}

namespace DGL
{
	void Loop() { Library.IsPaused = false; }
	void NoLoop() { Library.IsPaused = true; }
	void ToggleLoop() { Library.IsPaused = not Library.IsPaused; }
	bool IsLooping() { return not Library.IsPaused; }
	void Redraw() { Library.UserRequestedRedraw = true; }

	void PushLayer(OffscreenGraphicsLayer* layer) { Library.GraphicsLayerStack->PushLayer(layer); }
	void PopLayer() { Library.GraphicsLayerStack->PopLayer(); }
	GraphicsLayer& PeekLayer() { return Library.GraphicsLayerStack->PeekLayer(); }

	std::unique_ptr<OffscreenGraphicsLayer> CreateGraphics(const uint32_t width, const uint32_t height)
	{
		return OffscreenGraphicsLayer::Create(
			{ width, height },
			*Library.RendererFacade,
			*Library.ShapeFactory,
			*Library.BlendModeActivator
		);
	}

	void PushState() { PeekLayer().PushState(); }
	void PopState() { PeekLayer().PopState(); }
	RenderState& PeekState() { return PeekLayer().PeekState(); }

	void PushTransform() { PeekLayer().PushTransform(); }
	void PopTransform() { PeekLayer().PopTransform(); }
	Math::Matrix4x4& PeekTransform() { return PeekLayer().PeekTransform(); }
	void ResetTransform() { PeekLayer().ResetTransform(); }

	void Translate(const float x, const float y) { PeekLayer().Translate(x, y); }
	void Scale(const float x, const float y) { PeekLayer().Scale(x, y); }
	void Rotate(const Math::Angle angle) { PeekLayer().Rotate(angle); }
	void Skew(const Math::Angle angleX, const Math::Angle angleY) { PeekLayer().Skew(angleX, angleY); }

	void Fill(const Renderer::Color color) { PeekLayer().Fill(color); }
	void Stroke(const Renderer::Color color) { PeekLayer().Stroke(color); }
	void StrokeWeight(const float strokeWeight) { PeekLayer().StrokeWeight(strokeWeight); }

	void NoFill() { PeekLayer().NoFill(); }
	void NoStroke() { PeekLayer().NoStroke(); }

	void SetBlend(const Blending::BlendMode& blendMode) { PeekLayer().SetBlendMode(blendMode); }
	void SetRectMode(const RectMode& rectMode) { PeekLayer().SetRectMode(rectMode); }
	void SetImageMode(const RectMode& rectMode) { PeekLayer().SetImageMode(rectMode); }
	void SetEllipseMode(const EllipseMode& ellipseMode) { PeekLayer().SetEllipseMode(ellipseMode); }
	void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) { PeekLayer().SetSegmentCountMode(segmentCountMode); }

	void Background(const Renderer::Color color) { PeekLayer().Background(color); }
	void Rect(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Rect(x1, y1, x2, y2); }
	void Quad(const float x1, const float y1, const float xy2) { Rect(x1, y1, xy2, xy2); }
	void Ellipse(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Ellipse(x1, y1, x2, y2); }
	void Circle(const float x1, const float y1, const float xy2) { Ellipse(x1, y1, xy2, xy2); }
	void Point(const float x, const float y) { PeekLayer().Point(x, y); }
	void Line(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Line(x1, y1, x2, y2); }
	void Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { PeekLayer().Triangle(x1, y1, x2, y2, x3, y3); }
	void Image(const Texture::Texture& texture, const float x1, const float y1, const float x2, const float y2) { PeekLayer().Image(texture, x1, y1, x2, y2); }
}