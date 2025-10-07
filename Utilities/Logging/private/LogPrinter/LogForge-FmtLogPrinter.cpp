// Project Name : Logging
// File Name    : Logging-FmtLogPrinter.cpp
// Author       : Felix Busch
// Created Date : 2025/09/16

module;

#include <string>
#include <format>
#include <chrono>
#include <ranges>
#include <string_view>

module LogForge;

namespace LogForge
{
	[[nodiscard]] constexpr std::string_view LogLevelToString(const LogLevel level)
	{
		switch (level)
		{
			using enum LogLevel;
			case Trace:   return "TRACE";
			case Debug: return "DEBUG";
			case Info: return "INFO";
			case Warning: return "WARNING";
			case Error: return "ERROR";
			default: return "UNKNOWN";
		}
	}

	[[nodiscard]] constexpr std::string_view GetColorForLogLevel(const LogLevel level)
	{
		switch (level)
		{
			using enum LogLevel;
			case Trace:   return "\033[37m"; // White
			case Debug: return "\033[36m"; // Cyan
			case Info: return "\033[32m"; // Green
			case Warning: return "\033[33m"; // Yellow
			case Error: return "\033[31m"; // Red
			default: return "\033[0m"; // Reset
		}
	}

	[[nodiscard]] std::vector<std::string> Split(const std::string& source, const char delimiter)
	{
		std::vector<std::string> result;

		std::stringstream ss;
		ss << source;

		std::string line;
		while (std::getline(ss, line, delimiter))
		{
			result.push_back(line);
		}

		return result;
	}

	inline static constexpr std::string_view RESET_COLOR = "\033[0m";

	LogOutputEvent FmtLogPrinter::Print(const LogEvent& event)
	{
		// The output should be formatted foloowing the fmt-style:
		// [TIMESTAMP] [LEVEL] MESSAGE

		const auto timestamp = std::format("{:%d.%m.%Y %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(event.Timestamp));
		const auto level = LogLevelToString(event.Level);

		std::vector<std::string> messageLines = Split(event.Message, '\n');
		for (std::string& line : messageLines)
		{
			line = std::format("[{}] [{}] {}{}{}", timestamp, level, GetColorForLogLevel(event.Level), line, RESET_COLOR);
		}

		return LogOutputEvent {
			.Lines = std::move(messageLines),
			.Origin = event
		};
	}
}