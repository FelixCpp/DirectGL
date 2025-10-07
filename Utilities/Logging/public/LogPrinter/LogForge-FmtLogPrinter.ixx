// Project Name : Logging
// File Name    : Logging-FmtLogPrinter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:FmtLogPrinter;

import :LogPrinter;

export namespace LogForge
{
	struct FmtLogPrinter : LogPrinter
	{
		LogOutputEvent Print(const LogEvent& event) override;
	};
}