// Project Name : Logging
// File Name    : Logging-ProductionLogFilter.cpp
// Author       : Felix Busch
// Created Date : 2025/09/15

module LogForge;

namespace LogForge
{
	bool ProductionLogFilter::ShouldLog(const LogEvent& event)
	{
		return false; // In production, we do not log any events by default.
	}
}