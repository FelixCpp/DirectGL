// Project Name : Logging
// File Name    : Logging-ProductionFilter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:ProductionLogFilter;

import :LogFilter;

export namespace LogForge
{
	struct ProductionLogFilter : LogFilter
	{
		[[nodiscard]] bool ShouldLog(const LogEvent& event) override;
	};
}