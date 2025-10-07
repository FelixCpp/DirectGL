// Project Name : DirectGL-LibraryLogger
// File Name    : LibraryLogger-LibraryLogger.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <string>
#include <chrono>

export module DirectGL.LibraryLogger:LibraryLogger;

import Logging;

export namespace DGL::LibraryLogger
{
	class LibraryLogger
	{
	public:

		LibraryLogger();

		void SetLogger(Logger* logger);

		void Log(
			LogLevel level,
			const std::string& message,
			const std::chrono::system_clock::time_point& timestamp = std::chrono::system_clock::now()
		) const;

	private:

		Logger* m_Logger;

	};
}