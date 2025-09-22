// Project Name : Logging
// File Name    : Logging-FmtLogPrinter.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module Logging:FmtLogPrinter;

import :LogPrinter;

export struct FmtLogPrinter : LogPrinter
{
	LogOutputEvent Print(const LogEvent& event) override;
};