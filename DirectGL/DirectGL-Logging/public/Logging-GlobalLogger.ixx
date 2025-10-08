// Project Name : DirectGL-Logging
// File Name    : Logging-GlobalLogger.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <memory>
#include <string>
#include <chrono>

export module DirectGL.Logging:GlobalLogger;

import LogForge;

export namespace DGL::Logging
{
	void SetLogger(std::weak_ptr<LogForge::Logger> logger);
	void Log(LogForge::LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
	void Trace(const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
	void Info(const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
	void Error(const std::string& message, const std::chrono::system_clock::time_point& timepoint = std::chrono::system_clock::now());
}