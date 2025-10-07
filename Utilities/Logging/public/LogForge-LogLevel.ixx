// Project Name : Logging
// File Name    : Logging-LogLevel.ixx
// Author       : Felix Busch
// Created Date : 2025/09/15

export module LogForge:LogLevel;

export namespace LogForge
{
	enum class LogLevel
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
	};
}