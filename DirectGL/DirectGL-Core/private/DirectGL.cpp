module;

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>
#include <array>

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
			const auto [windowWidth, windowHeight] = static_cast<Math::Float2>(GetWindowSize());

			Library.Renderer = std::make_shared<Renderer2D>();
			Library.MainGraphicsLayer = std::make_unique<MainGraphicsLayer>(Library.Renderer, Math::FloatBoundary::FromLTWH(0.0f, 0.0f, windowWidth, windowHeight));

			Library.Sketch = factory();
			if (Library.Sketch == nullptr or not Library.Sketch->Setup())
			{
				Error("Couldn't setup the sketch");
				return;
			}

			Library.Window->SetVisible(true);
			Library.FrameRateLimiter.SetTargetFrameRate(160);

			std::chrono::duration<float> deltaTime{ 0.0f };
			auto lastFrameTime = std::chrono::high_resolution_clock::now();
			while (not Library.CloseRequested)
			{
				Library.FrameRateLimiter.BeginFrame();

				// Before we process events, we need to update the input listener
				Library.InputListener.Update();

				// Poll all events from the window
				while (const auto event = Library.Window->PollEvent())
				{
					event->Visit(
						[](const WindowEvent::Closed&)
						{
							Info("Window close event received");
							if (Library.HandleCloseAutomatically) Quit();
						},
						[&](const WindowEvent::Resized& resizeEvent)
						{
							Library.MainGraphicsLayer->SetViewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(resizeEvent.Width), static_cast<float>(resizeEvent.Height)));

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

					Library.MainGraphicsLayer->BeginDraw();
					Library.Sketch->Draw(deltaTime.count());
					Library.MainGraphicsLayer->EndDraw();

					// Present the rendered frame on screen
					Library.Context->SwapBuffers();

					const auto now = std::chrono::high_resolution_clock::now();
					deltaTime = now - lastFrameTime;
					lastFrameTime = now;
				}

				// Increment the number of frames processed
				++Library.FrameCount;
				Library.FrameRateLimiter.EndFrame();
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
	void SetAutoCloseEnabled(const bool enabled) { Library.HandleCloseAutomatically = enabled; }
	bool IsAutoCloseEnabled() { return Library.HandleCloseAutomatically; }
	void SetVerticalSyncEnabled(const bool enabled) { Library.Context->SetVerticalSyncEnabled(enabled); }
	bool IsVerticalSyncEnabled() { return Library.Context->IsVerticalSyncEnabled(); }
	void SetTargetFrameRate(const uint32_t frameRate) { Library.FrameRateLimiter.SetTargetFrameRate(frameRate); }
	void SetTargetFrameRateDisabled() { SetTargetFrameRate(0); }
	uint32_t GetTargetFrameRate() { return Library.FrameRateLimiter.GetTargetFrameRate(); }
	uint32_t GetCurrentFrameRate() { return Library.FrameRateLimiter.GetCurrentFrameRate(); }
}

namespace DGL
{
	GraphicsLayer& PeekLayer() { return *Library.MainGraphicsLayer; }

	void PushStyle(const bool extendCurrentStyle) { PeekLayer().PushStyle(extendCurrentStyle); }
	void PopStyle() { PeekLayer().PopStyle(); }
	RenderStyle& PeekStyle() { return PeekLayer().PeekStyle(); }

	const Math::FloatBoundary& GetViewport() { return Library.MainGraphicsLayer->GetViewport(); }

	void PushMatrix(const bool extendCurrentMatrix) { PeekLayer().PushMatrix(extendCurrentMatrix); }
	void PopMatrix() { PeekLayer().PopMatrix(); }
	Math::Matrix4x4& PeekMatrix() { return PeekLayer().PeekMatrix(); }

	void Translate(const float x, const float y) { PeekLayer().Translate(x, y); }
	void Rotate(const Math::Angle angle) { PeekLayer().Rotate(angle); }
	void Scale(const float scaleX, const float scaleY) { PeekLayer().Scale(scaleX, scaleY); }
	void Shear(const Math::Angle shearX, const Math::Angle shearY) { PeekLayer().Shear(shearX, shearY); }

	void SetRectMode(const RectMode& mode) { PeekLayer().SetRectMode(mode); }
	void SetEllipseMode(const EllipseMode& mode) { PeekLayer().SetEllipseMode(mode); }
	void SetEllipseSegmentsMode(const EllipseSegmentsMode& mode) { PeekLayer().SetEllipseSegmentsMode(mode); }

	void SetFillColor(const color_t color) { PeekLayer().SetFillColor(color); }
	void SetFillColorDisabled() { PeekLayer().SetFillColorDisabled(); }

	void SetStrokeColor(const color_t color) { PeekLayer().SetStrokeColor(color); }
	void SetStrokeColorDisabled() { PeekLayer().SetStrokeColorDisabled(); }
	void SetStrokeWeight(const float weight) { PeekLayer().SetStrokeWeight(weight); }
	void SetStrokeJoin(const StrokeJoin joinStyle) { PeekLayer().SetStrokeJoin(joinStyle); }
	void SetStrokeCap(const StrokeCap strokeCap) { PeekLayer().SetStrokeCap(strokeCap); }

	void SetBlendMode(const BlendMode& blendMode) { PeekLayer().SetBlendMode(blendMode); }
	void SetClipRectMode(const RectMode& mode) { PeekLayer().SetClipRectMode(mode); }
	void SetClipRect(const float x1, const float y1, const float x2, const float y2) { PeekLayer().SetClipRect(x1, y1, x2, y2); }
	void SetClipRectDisabled() { PeekLayer().SetClipRectDisabled(); }

	void SetTextSize(const float textSize) { PeekLayer().SetTextSize(textSize); }
	void SetTextFont(Font* font) { PeekLayer().SetTextFont(font); }
	void SetTextAlign(const TextAlignment alignment) { PeekLayer().SetTextAlign(alignment); }

	void SetImageOpacity(const float opacity) { color_t& tint = PeekStyle().ImageTint; SetImageTint({ tint.X, tint.Y, tint.Z, opacity }); }
	void SetImageAlpha(const uint8_t alpha) { SetImageOpacity(static_cast<float>(alpha) / 255.0f); }

	void SetImageTint(const color_t tint) { PeekLayer().SetImageTint(tint); }
	void SetImageMode(const RectMode& mode) { PeekLayer().SetImageMode(mode); }
	void SetImageSampler(const ImageSampler* sampler) { PeekLayer().SetImageSampler(sampler); }

	void SetShader(Shader* shader) { PeekLayer().SetShader(shader); }

	void BeginShape(const ShapeMode mode) { PeekLayer().BeginShape(mode); }
	void EndShape(const ShapeClosingMode mode) { PeekLayer().EndShape(mode); }
	void Vertex(const float x, const float y) { PeekLayer().Vertex(x, y); }

	void Background(const color_t color) { PeekLayer().Background(color); }
	void Rect(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Rect(x1, y1, x2, y2); }
	void RoundedRect(const float x1, const float y1, const float x2, const float y2, const Math::BorderRadius& borderRadius) { PeekLayer().RoundedRect(x1, y1, x2, y2, borderRadius); }
	void Ellipse(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Ellipse(x1, y1, x2, y2); }
	void Circle(const float x1, const float y1, const float xy2) { Ellipse(x1, y1, xy2, xy2); }
	void Point(const float x, const float y) { PeekLayer().Point(x, y); }
	void Line(const float x1, const float y1, const float x2, const float y2) { PeekLayer().Line(x1, y1, x2, y2); }
	void Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { PeekLayer().Triangle(x1, y1, x2, y2, x3, y3); }
	void Text(const std::string_view text, const float x, const float y) { return PeekLayer().Text(text, x, y); }
	void Image(const Image2D& image, const float x1, const float y1) { Image(image, x1, y1, static_cast<float>(image.GetSize().X), static_cast<float>(image.GetSize().Y)); }
	void Image(const Image2D& image, const float x1, const float y1, const float x2, const float y2) { Image(image, x1, y1, x2, y2, 0.0f, 0.0f, static_cast<float>(image.GetSize().X), static_cast<float>(image.GetSize().Y)); }
	void Image(const Image2D& image, const float x1, const float y1, const float x2, const float y2, const float sourceLeft, const float sourceTop, const float sourceWidth, const float sourceHeight) { PeekLayer().Image(image, x1, y1, x2, y2, sourceLeft, sourceTop, sourceWidth, sourceHeight); }
}