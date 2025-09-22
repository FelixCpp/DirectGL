// Project Name : Logging
// File Name    : Logging-LogEvent.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

module;

#include <string>
#include <chrono>

export module Logging:LogEvent;

import :LogLevel;

export struct LogEvent
{
	LogLevel Level;
	std::string Message;
	std::chrono::system_clock::time_point Timestamp;
	//std::stacktrace StackTrace;
};