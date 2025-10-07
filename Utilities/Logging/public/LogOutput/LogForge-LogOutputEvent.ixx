// Project Name : Logging
// File Name    : Logging-LogOutputEvent.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

module;

#include <vector>
#include <string>

export module LogForge:LogOutputEvent;

import :LogEvent;

export namespace LogForge
{
	struct LogOutputEvent
	{
		std::vector<std::string> Lines;
		LogEvent Origin;
	};
}