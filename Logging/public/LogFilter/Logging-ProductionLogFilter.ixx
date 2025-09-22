// Project Name : Logging
// File Name    : Logging-ProductionFilter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module Logging:ProductionLogFilter;

import :LogFilter;

export struct ProductionLogFilter : LogFilter
{
	[[nodiscard]] bool ShouldLog(const LogEvent& event) override;
};