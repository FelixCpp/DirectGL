// Project Name : Logging
// File Name    : Logging-ProductionLogFilter.cpp
// Author       : Felix Busch
// Created Date : 2025/09/15

module Logging;

bool ProductionLogFilter::ShouldLog(const LogEvent& event)
{
	return false;
}