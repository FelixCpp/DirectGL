// Project Name : Logging
// File Name    : LogForge-Logger.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <string>
#include <chrono>

export module LogForge:Logger;

import :LogLevel;

export namespace LogForge
{
	struct Logger
	{
		virtual ~Logger() = default;
		virtual void Log(LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timepoint) = 0;
	};
}