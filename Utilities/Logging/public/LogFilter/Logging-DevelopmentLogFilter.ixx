// Project Name : Logging
// File Name    : Logging-DevelopmentLogFilter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module Logging:DevelopmentLogFilter;

import :LogFilter;

export struct DevelopmentLogFilter : LogFilter
{
	[[nodiscard]] bool ShouldLog(const LogEvent& event) override;
};