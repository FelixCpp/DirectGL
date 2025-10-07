module;

#include <string>
#include <chrono>

module DirectGL.Renderer;

import LogForge;

import :Logging;

namespace DGL::Renderer
{
	void Log(const LogForge::LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& time)
	{
		if (const auto logger = s_Logger.lock())
		{
			logger->Log(level, message, time);
		}
	}

	void Trace(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogForge::LogLevel::Trace, message, time); }
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogForge::LogLevel::Debug, message, time); }
	void Info(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogForge::LogLevel::Info, message, time); }
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogForge::LogLevel::Warning, message, time); }
	void Error(const std::string& message, const std::chrono::system_clock::time_point& time) { Log(LogForge::LogLevel::Error, message, time); }
}