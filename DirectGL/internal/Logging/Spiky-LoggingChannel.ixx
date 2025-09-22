// Project Name : DGL
// File Name    : DGL-LoggingChannel.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

module;

#include <memory>
#include <string>
#include <chrono>

export module DGL:LoggingChannel;

import Logging;

import :LoggingStartupTask;

namespace DGL
{
	class LoggingChannel
	{
	public:

		explicit LoggingChannel(std::unique_ptr<Logger> logger);
		void Submit(LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timestamp);
		const std::shared_ptr<LoggingStartupTask>& GetStartupTask() const;

	private:

		std::shared_ptr<LoggingStartupTask> m_LoggingTask;

	};
}
