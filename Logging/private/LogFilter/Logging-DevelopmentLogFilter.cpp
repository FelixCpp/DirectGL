// Project Name : Logging
// File Name    : Logging-DevelopmentLogFilter.cpp
// Author       : Felix Busch
// Created Date : 2025/09/15

module Logging;

bool DevelopmentLogFilter::ShouldLog(const LogEvent &event)
{
	return true;
}