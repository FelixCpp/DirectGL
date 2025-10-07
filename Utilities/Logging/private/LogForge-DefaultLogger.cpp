module;

#include <type_traits>
#include <memory>
#include <string>
#include <chrono>

module LogForge;

namespace LogForge
{
	DefaultLogger::DefaultLogger(std::unique_ptr<LogFilter> filter, std::unique_ptr<LogPrinter> printer, std::unique_ptr<LogOutput> output) :
		m_Filter(std::move(filter)),
		m_Printer(std::move(printer)),
		m_Output(std::move(output))
	{
	}

	void DefaultLogger::Log(const LogLevel level, const std::string& message, const std::chrono::system_clock::time_point& timestamp)
	{
		const LogEvent event =
		{
			.Level = level,
			.Message = message,
			.Timestamp = timestamp,
		};

		if (m_Filter->ShouldLog(event))
		{
			const auto output = m_Printer->Print(event);
			m_Output->Output(output);
		}
	}
}