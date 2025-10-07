// Project Name : Logging
// File Name    : Logging-DevelopmentLogFilter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:DevelopmentLogFilter;

import :LogFilter;

export namespace LogForge
{
	class DevelopmentLogFilter : public LogFilter
	{
	public:

		constexpr explicit DevelopmentLogFilter(LogLevel minSeverity = LogLevel::Trace);
		[[nodiscard]] bool ShouldLog(const LogEvent& event) override;

	private:

		LogLevel m_MinSeverityLevel;
	};
}

namespace LogForge
{
	constexpr DevelopmentLogFilter::DevelopmentLogFilter(const LogLevel minSeverity):
		m_MinSeverityLevel(minSeverity)
	{
	}
}