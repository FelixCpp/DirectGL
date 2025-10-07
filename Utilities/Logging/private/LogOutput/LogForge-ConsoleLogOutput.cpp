// Project Name : Logging
// File Name    : Logging-ConsoleLogOutput.cpp
// Author       : Felix Busch
// Created Date : 2025/09/15

module;

#include <print>

module LogForge;

namespace LogForge
{
	void ConsoleLogOutput::Output(const LogOutputEvent& event)
	{
		for (const auto& line : event.Lines)
		{
			std::println("{}", line);
		}
	}
}