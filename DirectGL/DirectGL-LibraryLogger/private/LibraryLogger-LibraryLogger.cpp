module;

#include <memory>

module DirectGL.LibraryLogger;

namespace DGL::LibraryLogger
{
	static auto s_FallbackLogger = Logger(
		std::make_unique<DevelopmentLogFilter>(),
		std::make_unique<FmtLogPrinter>(),
		std::make_unique<ConsoleLogOutput>()
	);

	void LibraryLogger::SetLogger(Logger* logger)
	{
		m_Logger = logger;
	}

	void LibraryLogger::Log(const LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timestamp) const
	{
		if (m_Logger != nullptr)
		{
			m_Logger->Log(level, message, timestamp);
		} else
		{
			s_FallbackLogger.Log(level, message, timestamp);
		}
	}
}