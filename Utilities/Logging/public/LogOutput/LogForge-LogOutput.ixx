// Project Name : Logging
// File Name    : Logging-LogOutput.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:LogOutput;

import :LogOutputEvent;

export namespace LogForge
{
	struct LogOutput
	{
		virtual ~LogOutput() = default;
		virtual void Output(const LogOutputEvent& event) = 0;
	};
}