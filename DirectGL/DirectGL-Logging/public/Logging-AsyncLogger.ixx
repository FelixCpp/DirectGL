// Project Name : DirectGL-Logging
// File Name    : Logging-AsyncLogger.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <string>
#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

export module DirectGL.Logging:AsyncLogger;

import LogForge;
import Startup;

export namespace DGL::Logging
{
	class AsyncLogger : public LogForge::Logger, public Startup::StartupTask
	{
	private:

		struct LogEntry
		{
			LogForge::LogLevel Level;
			std::string Message;
			std::chrono::system_clock::time_point Timestamp;
		};

	public:

		explicit AsyncLogger(std::unique_ptr<Logger> logger);
		void Log(LogForge::LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timepoint) override;
		Continuation Setup() override;
		void Teardown() override;

	private:

		std::jthread m_LoggingThread;
		std::mutex m_SubmitMutex;
		std::condition_variable m_SubmitCondition;

		std::queue<LogEntry> m_SubmittedEntries;
		std::unique_ptr<LogForge::Logger> m_Logger;

	};
}