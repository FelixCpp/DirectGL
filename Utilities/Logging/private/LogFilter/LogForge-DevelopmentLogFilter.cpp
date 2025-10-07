// Project Name : Logging
// File Name    : Logging-DevelopmentLogFilter.cpp
// Author       : Felix Busch
// Created Date : 2025/09/15

module LogForge;

namespace LogForge
{
	bool DevelopmentLogFilter::ShouldLog(const LogEvent& event)
	{
#ifndef NDEBUG
		return static_cast<int>(event.Level) >= static_cast<int>(m_MinSeverityLevel);
#else
		return false; //< Disable all logging in release builds
#endif
	}
}