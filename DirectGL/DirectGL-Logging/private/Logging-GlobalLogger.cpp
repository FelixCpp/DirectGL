module;

#include <string>
#include <chrono>

module DirectGL.Logging;

namespace DGL::Logging
{
	void SetLogger(std::weak_ptr<LogForge::Logger> logger)
	{
		g_RootLogger = std::move(logger);
	}

	void Log(const LogForge::LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		if (const auto logger = g_RootLogger.lock())
		{
			logger->Log(level, message, timepoint);
		}
	}

	void Trace(const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		Log(LogForge::LogLevel::Trace, message, timepoint);
	}

	void Debug(const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		Log(LogForge::LogLevel::Debug, message, timepoint);
	}

	void Info(const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		Log(LogForge::LogLevel::Info, message, timepoint);
	}

	void Warning(const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		Log(LogForge::LogLevel::Warning, message, timepoint);
	}

	void Error(const std::string& message, const std::chrono::system_clock::time_point& timepoint)
	{
		Log(LogForge::LogLevel::Error, message, timepoint);
	}
}