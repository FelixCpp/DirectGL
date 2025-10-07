// Project Name : Logging
// File Name    : Logging-LogPrinter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:LogPrinter;

import :LogEvent;
import :LogOutputEvent;

export namespace LogForge
{
	struct LogPrinter
	{
		virtual ~LogPrinter() = default;
		virtual LogOutputEvent Print(const LogEvent& event) = 0;
	};
}
