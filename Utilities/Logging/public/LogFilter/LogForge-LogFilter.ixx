// Project Name : Logging
// File Name    : Logging-LogFilter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:LogFilter;

import :LogEvent;

export namespace LogForge
{
	struct LogFilter
	{
		virtual ~LogFilter() = default;
		[[nodiscard]] virtual bool ShouldLog(const LogEvent& event) = 0;
	};
}