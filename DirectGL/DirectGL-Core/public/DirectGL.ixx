module;

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <chrono>
#include <filesystem>

export module DGL;

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

export namespace DGL
{
	using Color = Renderer::Color;
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

// Logging
import :LoggingChannel;
import :LoggingStartupTask;

import :WindowStartupTask;

struct SpikyLibrary
{
	std::shared_ptr<System::MonitorProvider>		MonitorProvider;	//!< The monitor provider to use
	std::shared_ptr<DGL::ContextWrapper>			Context;			//!< The WGL configuration task
	std::shared_ptr<DGL::WindowWrapper>				Window;				//!< The Core window of the sketch

	std::unique_ptr<DGL::Sketch>					Sketch;				//!< The sketch provided by the user
	std::unique_ptr<DGL::LoggingChannel>			LoggingChannel;		//!< The logging channel to use
};

module :private;

SpikyLibrary Library;