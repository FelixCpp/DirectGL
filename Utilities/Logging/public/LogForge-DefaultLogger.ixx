// Project Name : Logging
// File Name    : Logging-DefaultLogger.ixx
// Author       : Felix Busch
// Created Date : 2025/09/16

module;

#include <memory>
#include <string>
#include <chrono>

export module LogForge:DefaultLogger;

import :Logger;
import :LogLevel;
import :LogFilter;
import :LogPrinter;
import :LogOutput;

export namespace LogForge
{
	class DefaultLogger : public Logger
	{
	public:

		explicit DefaultLogger(
			std::unique_ptr<LogFilter> filter,
			std::unique_ptr<LogPrinter> printer,
			std::unique_ptr<LogOutput> output
		);

		void Log(
			LogLevel level,
			const std::string& message,
			const std::chrono::system_clock::time_point& timestamp = std::chrono::system_clock::now()
		) override;

	private:

		std::unique_ptr<LogFilter> m_Filter;
		std::unique_ptr<LogPrinter> m_Printer;
		std::unique_ptr<LogOutput> m_Output;

	};
}