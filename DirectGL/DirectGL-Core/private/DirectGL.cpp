module;

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include <glad/gl.h>

module DirectGL;

import LogForge;

/// <summary>
/// Sketch interface to be implemented by the user
/// </summary>
namespace DGL
{
	void LaunchImpl(const std::function<std::unique_ptr<Sketch>()>& factory)
	{
		Library.Logger = std::make_unique<AsyncLogger>(std::make_unique<LogForge::DefaultLogger>(
			std::make_unique<LogForge::DevelopmentLogFilter>(),
			std::make_unique<LogForge::FmtLogPrinter>(),
			std::make_unique<LogForge::ConsoleLogOutput>()
		));

		Logging::SetLogger(Library.Logger);

		Library.Context = std::make_unique<ContextWrapper>([] { return Library.Window.get(); });
		Library.Window = std::make_unique<WindowWrapper>(std::make_shared<MonitorProviderCache>(CreateDefaultMonitorProvider()));

		AppStartup startup;
		startup.AddStartupTask(Library.Logger);
		startup.AddStartupTask(std::make_shared<ConfigureDPIStartupTask>());
		startup.AddStartupTask(Library.Window);
		startup.AddStartupTask(Library.Context);
		startup.AddStartupTask(std::make_shared<ConfigureGladStartupTask>());

		startup.Run([&factory]
		{
			DepthProvider provider;
			Library.ShapeBuilder = std::make_unique<ShapeBuilder>(provider);
			Library.MeshRenderer = std::make_unique<MeshRenderer>();

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
							glViewport(0, 0, resizeEvent.Width, resizeEvent.Height);

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

					const auto [w, h] = GetWindowSize();

					provider.Reset();
					Library.MeshRenderer->BeginDraw(Math::Matrix4x4::Orthographic(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, w, h), -1.0f, 1.0f));
					Library.Sketch->Draw(deltaTime.count());
					Library.MeshRenderer->EndDraw();

					// Present the rendered frame on screen
					Library.Context->SwapBuffers();

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
		const Math::Uint2 newWindowSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		if (recenter)
		{
			Library.Window->SetSizeAndRecenter(newWindowSize);
		} else
		{
			Library.Window->SetSize(newWindowSize);
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
	void Loop() { Library.IsPaused = false; }
	void NoLoop() { Library.IsPaused = true; }
	void ToggleLoop() { Library.IsPaused = not Library.IsPaused; }
	bool IsLooping() { return not Library.IsPaused; }
	void Redraw() { Library.UserRequestedRedraw = true; }
}

namespace DGL
{
	void PushStyle()
	{
		Library.RenderStyleStack.PushStyle();
	}

	void PopStyle()
	{
		Library.RenderStyleStack.PopStyle();
	}

	RenderStyle& GetCurrentRenderStyle()
	{
		return Library.RenderStyleStack.PeekStyle();
	}

	void SetFillColor(const color_t color)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.FillColor = color;
		style.IsFillEnabled = true;
	}

	void SetStrokeColor(const color_t color)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.StrokeColor = color;
		style.IsStrokeEnabled = true;
	}

	void SetStrokeWeight(const float weight)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.StrokeWeight = weight;
	}

	void SetStrokeStartCap(const StrokeCap startCap)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.StartCap = startCap;
	}

	void SetStrokeEndCap(const StrokeCap endCap)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.EndCap = endCap;
	}

	void SetStrokeJoin(const StrokeJoin joinStyle)
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.JoinStyle = joinStyle;
	}

	void SetFillDisabled()
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.IsFillEnabled = false;
	}

	void SetStrokeDisabled()
	{
		RenderStyle& style = GetCurrentRenderStyle();
		style.IsStrokeEnabled = false;
	}

	void BeginShape(const ShapeMode mode)
	{
		Library.ShapeBuilder->Begin(mode);
	}

	void EndShape()
	{
		const RenderStyle& style = GetCurrentRenderStyle();

		const Shape shape = Library.ShapeBuilder->End({
			.StrokeWeight = style.StrokeWeight,
			.JoinStyle = style.JoinStyle,
			.StartCap = style.StartCap,
			.EndCap = style.EndCap,
			.IsStrokeEnabled = style.IsStrokeEnabled,
			.IsFillEnabled = style.IsFillEnabled,
			.ShouldCloseOutline = true,
		});

		Library.MeshRenderer->Submit(shape.FillShapes);
		Library.MeshRenderer->Submit(shape.StrokeShapes);
	}

	void Vertex(const float x, const float y)
	{
		const RenderStyle& style = GetCurrentRenderStyle();

		Library.ShapeBuilder->AddVertex({
			.Position = { x, y },
			.FillColor = style.FillColor,
			.StrokeColor = style.StrokeColor
		});
	}

	void Background(const color_t color)
	{
		const auto [width, height] = static_cast<Math::Float2>(GetWindowSize());

		const Mesh mesh = MeshBuilder::GenerateQuadMesh(
			std::array {
				Math::Float2{ 0.0f, 0.0f },
				Math::Float2{ width, 0.0f },
				Math::Float2{ width, height },
				Math::Float2{ 0.0f, height }
			},
			std::array { color, color, color, color },
			0.0f
		);

		Library.MeshRenderer->Submit(std::array{ mesh });
	}

	void Rect(const float x1, const float y1, const float x2, const float y2)
	{
		BeginShape(ShapeMode::Quads);
		Vertex(x1, y1);
		Vertex(x2, y1);
		Vertex(x2, y2);
		Vertex(x1, y2);
		EndShape();
	}

	void Point(const float x, const float y)
	{
		BeginShape(ShapeMode::Points);
		Vertex(x, y);
		EndShape();
	}

	void Line(const float x1, const float y1, const float x2, const float y2)
	{
		BeginShape(ShapeMode::Lines);
		Vertex(x1, y1);
		Vertex(x2, y2);
		EndShape();
	}
}