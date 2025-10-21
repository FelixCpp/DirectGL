module;

#include <glad/gl.h>
#include <glad/wgl.h>

#include <bit>
#include <format>
#include <sstream>

module DirectGL;

namespace DGL
{
	Startup::StartupTask::Continuation ConfigureGladStartupTask::Setup()
	{
		if (not gladLoaderLoadGL())
		{
			Error("Failed to initialize GLAD");
			return Abort;
		}

		const GLubyte* glVersion = glGetString(GL_VERSION);
		Debug(std::format("OpenGL version: {}", glVersion ? reinterpret_cast<const char*>(glVersion) : "Unknown"));

		const GLubyte* glRenderer = glGetString(GL_RENDERER);
		Debug(std::format("OpenGL renderer: {}", glRenderer ? reinterpret_cast<const char*>(glRenderer) : "Unknown"));

		const GLubyte* glVendor = glGetString(GL_VENDOR);
		Debug(std::format("OpenGL vendor: {}", glVendor ? reinterpret_cast<const char*>(glVendor) : "Unknown"));

		const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		Debug(std::format("OpenGL GLSL version: {}", glslVersion ? reinterpret_cast<const char*>(glslVersion) : "Unknown"));

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			const std::string_view sourceString = [source] {
				switch (source)
				{
					case GL_DEBUG_SOURCE_API: return "API";
					case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
					case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
					case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
					case GL_DEBUG_SOURCE_APPLICATION: return "Application";
					case GL_DEBUG_SOURCE_OTHER: return "Other";
					default: return "Unknown";
				}
			}();

			const std::string_view typeString = [type] {
				switch (type)
				{
					case GL_DEBUG_TYPE_ERROR: return "Error";
					case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
					case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
					case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
					case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
					case GL_DEBUG_TYPE_MARKER: return "Marker";
					case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
					case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
					case GL_DEBUG_TYPE_OTHER: return "Other";
					default: return "Unknown";
				}
			}();

			const std::string_view severityString = [severity] {
				switch (severity)
				{
					case GL_DEBUG_SEVERITY_HIGH: return "High";
					case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
					case GL_DEBUG_SEVERITY_LOW: return "Low";
					case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
					default: return "Unknown";
				}
			}();

			std::ostringstream ss;
			ss << "OpenGL Debug Message [" << id << "]: " << message << '\n';
			ss << "\tSource: " << sourceString << '\n';
			ss << "\tType: " << typeString << '\n';
			ss << "\tSeverity: " << severityString;

			Error(ss.str());
		}, nullptr);

		Info("Successfully initialized GLAD");
		return Continue;
	}

	void ConfigureGladStartupTask::Teardown()
	{
		
	}
}