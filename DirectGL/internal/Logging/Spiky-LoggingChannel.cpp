module;

#include <memory>
#include <string>
#include <chrono>

module DGL;

namespace DGL
{
	LoggingChannel::LoggingChannel(std::unique_ptr<Logger> logger):
		m_LoggingTask(std::make_shared<LoggingStartupTask>(std::move(logger)))
	{
	}

	void LoggingChannel::Submit(const LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timestamp)
	{
		m_LoggingTask->Submit({
			.Level = level,
			.Message = message,
			.Timestamp = timestamp
		});
	}

	const std::shared_ptr<LoggingStartupTask>& LoggingChannel::GetStartupTask() const
	{
		return m_LoggingTask;
	}
}