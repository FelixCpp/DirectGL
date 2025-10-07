// Project Name : DirectGL-Renderer
// File Name    : Renderer-Logging.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <string>
#include <chrono>

export module DirectGL.Renderer:Logging;

import LogForge;

namespace DGL::Renderer
{
	void Log(LogForge::LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Trace(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Debug(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Info(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Warning(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	void Error(const std::string& message, const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
}