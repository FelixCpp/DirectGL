module;

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <chrono>

export module DirectGL;

/////////////////////////////// - IMPORTS - ///////////////////////////////
///																		///
/// This Section contains all imports that are part of the public API	///
/// of the library.														///
///																		///
///////////////////////////////////////////////////////////////////////////

/// First we export external libraries that are required by the public API
/// of the library.
export import Math;
export import System.Window;

import LogForge;
import DirectGL.Renderer;
import DirectGL.Logging;
import DirectGL.Input;

/////////////////////////////// - IMPORTS - ///////////////////////////////
///																		///
/// This Section contains all imports that are part of the public API	///
/// of the library.														///
///																		///
///////////////////////////////////////////////////////////////////////////

/// <summary>
/// Sketch interface to be implemented by the user
/// </summary>
export namespace DGL
{
	struct Sketch
	{
		virtual ~Sketch() = default;
		virtual bool Setup() = 0;
		virtual void Event(const System::WindowEvent& event) = 0;
		virtual void Draw() = 0;
		virtual void Destroy() = 0;
	};

	int Launch(const std::function<std::unique_ptr<Sketch>()>& factory);
}

/// <summary>
/// Logging functionality.
/// </summary>
export namespace DGL
{
	using LogLevel = LogForge::LogLevel;

	void Log(LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Trace(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Info(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Error(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
}

export namespace DGL
{
	void Restart();
	void Restart(int exitCode);
	void Quit();
	void Quit(int exitCode);
	void SetExitCode(int exitCode);

	using KeyboardKey = Input::KeyboardKey;
	using MouseButton = Input::MouseButton;

	[[nodiscard]] bool IsKeyPressed(KeyboardKey key);
	[[nodiscard]] bool IsKeyDown(KeyboardKey key);
	[[nodiscard]] bool IsKeyReleased(KeyboardKey key);
	[[nodiscard]] bool IsMouseButtonPressed(MouseButton button);
	[[nodiscard]] bool IsMouseButtonDown(MouseButton button);
	[[nodiscard]] bool IsMouseButtonReleased(MouseButton button);
	[[nodiscard]] Math::Int2 GetMousePosition();
}

/// <summary>
/// Windowing and event handling
/// </summary>
export namespace DGL
{
	void ShowWindow();												  //!< Make the window visible to the user
	void HideWindow();												  //!< Hide the window from the user
	bool IsWindowVisible();											  //!< Get whether the window is currently visible to the user
	void SetWindowSize(int width, int height, bool recenter = true);  //!< Set the window size, optionally recentering it after resize on the primary monitor
	Math::Uint2 GetWindowSize();									  //!< Get the size of the content-area of the window
	void SetWindowPosition(int x, int y);							  //!< Set the window position in screen coordinates
	Math::Int2 GetWindowPosition();									  //!< Get the window position in screen coordinates
	void SetWindowTitle(std::string_view title);					  //!< Set the window title
	std::string GetWindowTitle();									  //!< Get the window title
}

export import :BlendMode;
export import :Color;
export import :RenderState;
export import :GraphicsLayer;
export import :OffscreenGraphicsLayer;

export namespace DGL
{
	void Loop();
	void NoLoop();
	void ToggleLoop();
	bool IsLooping();
	void Redraw();

	void PushState();
	void PopState();
	RenderState& PeekState();

	void Fill(Color color);
	void Stroke(Color color);
	void StrokeWeight(float strokeWeight);

	void NoFill();
	void NoStroke();

	void Blend(const BlendMode& blendMode);

	void Background(Color color);
	void Rect(float x1, float y1, float x2, float y2);
	void Ellipse(float x1, float y1, float x2, float y2);
	void Circle(float x1, float y1, float xy2);
	void Point(float x, float y);
	void Line(float x1, float y1, float x2, float y2);
	void Triangle(float x1, float y1, float x2, float y2, float x3, float y3);
}

//////////////////////////////// - Non-API - //////////////////////////////
///																		///
///	This Section contains type definitions and implementations that are	///
///	not part of the public API of the library.							///
///																		///
///////////////////////////////////////////////////////////////////////////

// DPI
import :ConfigureDPIStartupTask;

// Graphics
import :ConfigureGladStartupTask;
import :ContextWrapper;
import :RenderStateStack;

import :WindowStartupTask;
import :InputListener;

import :MainGraphicsLayer;

enum struct ExitType
{
	Quit,
	Restart,
};

struct DirectGLLibrary
{
	std::shared_ptr<System::MonitorProvider>		MonitorProvider;	//!< The monitor provider to use
	std::shared_ptr<DGL::ContextWrapper>			Context;			//!< The WGL configuration task
	std::shared_ptr<DGL::WindowWrapper>				Window;				//!< The Core window of the sketch

	std::unique_ptr<DGL::Sketch>					Sketch;				//!< The sketch provided by the user
	std::shared_ptr<DGL::Logging::AsyncLogger>		Logger;				//!< The logging channel to use

	DGL::InputListener									InputListener;		//!< The input listener to use

	std::unique_ptr<DGL::Renderer::ShapeFactory>		ShapeFactory;			//!< The shape factory to use
	std::unique_ptr<DGL::Renderer::VertexRenderer>		VertexRenderer;			//!< The vertex renderer to use
	std::unique_ptr<DGL::MainGraphicsLayer>				MainGraphicsLayer;		//!< The main graphics layer to use for rendering

	ExitType		ExitType = ExitType::Quit;		//!< The exit code to return on application shutdown
	int				ExitCode = 0;					//!< The return code to return on application shutdown
	bool			CloseRequested = false;			//!< Whether a restart of the application was requested
	uint64_t		FrameCount = 0;					//!< The number of frames that have been rendered since application start
	bool			IsPaused = false;				//!< Whether the application is currently paused
	bool			UserRequestedRedraw = false;	//!< Whether the user requested a redraw (call to Sketch::Draw function).
};

module :private;

DirectGLLibrary Library;