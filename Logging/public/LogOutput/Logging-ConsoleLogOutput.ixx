// Project Name : Logging
// File Name    : Logging-ConsoleLogOutput.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module Logging:ConsoleLogOutput;

import :LogOutput;

export struct ConsoleLogOutput : LogOutput
{
	void Output(const LogOutputEvent& event) override;
};