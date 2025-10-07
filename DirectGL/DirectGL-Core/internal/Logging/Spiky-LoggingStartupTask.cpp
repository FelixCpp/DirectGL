module;

#include <print>
#include <mutex>
#include <vector>

module DGL;

namespace DGL
{
	LoggingStartupTask::LoggingStartupTask(std::unique_ptr<LogForge::Logger> logger) :
		m_Logger(std::move(logger))
	{}

	void LoggingStartupTask::Submit(const LogEntry& entry)
	{
		{
			std::lock_guard lock(m_SubmitMutex);
			m_SubmittedEntries.push(entry);
		}

		m_SubmitCondition.notify_one();
	}

	Startup::StartupTask::Continuation LoggingStartupTask::Setup()
	{
		m_LoggingThread = std::jthread([this](const std::stop_token& token)
		{
			while (not token.stop_requested() or not m_SubmittedEntries.empty())
			{
				std::vector<LogEntry> batch;

				{
					std::unique_lock lock(m_SubmitMutex);

					// Wait until there is a new log entry or a stop is requested
					m_SubmitCondition.wait(lock, [this, &token]
					{
						return not m_SubmittedEntries.empty() or token.stop_requested();
					});

					while (not m_SubmittedEntries.empty())
					{
						batch.emplace_back(std::move(m_SubmittedEntries.front()));
						m_SubmittedEntries.pop();
					}
				}
				
				for (const auto& entry : batch)
				{
					m_Logger->Log(entry.Level, entry.Message, entry.Timestamp);
				}
			}
		});

		Info("Logging system initialized");
		return Continue;
	}

	void LoggingStartupTask::Teardown()
	{
		Info("Logging system deinitialized");

		if (m_LoggingThread.joinable())
		{
			m_LoggingThread.request_stop();

			// TODO(Felix): This is a hack to ensure all messages are processed before we exit
			// We should implement a proper shutdown mechanism
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			{
				std::lock_guard lock(m_SubmitMutex);
				// Nothing to do here but we do follow the pattern
			}

			m_SubmitCondition.notify_all();
		}
	}
}