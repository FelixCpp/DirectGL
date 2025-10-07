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

/// Public API of the Graphics module
export import :Brush;
export import :SolidColorBrush;
export import :TextureBrush;

export import :RenderTarget;
export import :WindowRenderTarget;
export import :OffscreenRenderTarget;

export import :BorderRadius;
export import :Color;
export import :Geometry;
export import :GraphicsAPI;
export import :LineCap;
export import :Radius;
export import :Renderer;
export import :RenderTarget;
export import :Shader;
export import :ShaderProgram;
export import :Texture;
export import :TextureSampler;

/////////////////////////////// - IMPORTS - ///////////////////////////////
///																		///
/// This Section contains all imports that are part of the public API	///
/// of the library.														///
///																		///
///////////////////////////////////////////////////////////////////////////

import Logging;

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

	int Launch(
		GraphicsAPI api,
		const std::function<std::unique_ptr<Sketch>()>& factory
	);
}

/// <summary>
/// Logging functionality.
/// </summary>
export namespace DGL
{
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

/// <summary>
/// Factory methods to create graphics resources
/// </summary>
export namespace DGL
{
	std::unique_ptr<OffscreenRenderTarget> CreateOffscreenRenderTarget(uint32_t width, uint32_t height);
	std::unique_ptr<Shader> CreateShader(std::string_view shaderSource, ShaderType type);
	std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
	std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath);
	std::unique_ptr<TextureSampler> CreateTextureSampler(TextureWrapMode wrapMode, TextureFilterMode filterMode);
	std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(Color color);
}

/// Retrieve internal DirectGL state
export namespace DGL
{
	Renderer& GetRenderer();
	RenderTarget& GetRenderTarget();
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

import :OpenGLSolidColorBrush;

import :OpenGLOffscreenRenderTarget;
import :OpenGLWindowRenderTarget;

import :OpenGLRenderer;
import :OpenGLShader;
import :OpenGLShaderProgram;
import :OpenGLTexture;
import :OpenGLTextureSampler;

import :GeometryFactory;
import :ResourceFactory;

import :Camera;
import :OpenGLTexture;
import :OpenGLTextureSampler;

import :ResourceFactory;
import :OpenGLResourceFactory;

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

	std::unique_ptr<DGL::Renderer> 					Renderer;			//!< The renderer to use
	std::unique_ptr<DGL::WindowRenderTarget>		RenderTarget;		//!< The render target to use

	std::unique_ptr<DGL::ResourceFactory>			ResourceFactory;	//!< The resource factory to use
};

module :private;

SpikyLibrary Library;