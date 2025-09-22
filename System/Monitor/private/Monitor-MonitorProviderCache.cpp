module;

#include <optional>

module System.Monitor;

namespace System
{

	MonitorProviderCache::MonitorProviderCache(std::shared_ptr<MonitorProvider> provider)
	{
		Reset(std::move(provider));
	}

	std::optional<Monitor> MonitorProviderCache::GetPrimaryMonitor() const
	{
		if (not m_PrimaryMonitor.has_value())
		{
			m_PrimaryMonitor = m_MonitorProvider->GetPrimaryMonitor();

			/*if (m_PrimaryMonitor.has_value())
			{
				const auto& [workLeft, workTop, workWidth, workHeight] = m_PrimaryMonitor->WorkArea;
				const auto& [areaLeft, areaTop, areaWidth, areaHeight] = m_PrimaryMonitor->Area;
				Debug("Primary Monitor received");
				Debug(std::format("  Name: {}", m_PrimaryMonitor->Name));
				Debug(std::format("  WorkArea: left: {}, top: {}, width: {}, height: {}", workLeft, workTop, workWidth, workHeight));
				Debug(std::format("  Area: left: {}, top: {}, width: {}, height: {}", areaLeft, areaTop, areaWidth, areaHeight));
				Debug(std::format("  Is Primary: {}", m_PrimaryMonitor->IsPrimary));
			}*/
		}

		return m_PrimaryMonitor;
	}

	std::vector<Monitor> MonitorProviderCache::GetAvailableMonitors() const
	{
		if (m_AvailableMonitors.empty())
		{
			m_AvailableMonitors = m_MonitorProvider->GetAvailableMonitors();

			/*Debug("Available Monitors received");
			Debug(std::format("  Count: {}", m_AvailableMonitors.size()));
			for (const auto& monitor : m_AvailableMonitors)
			{
				const auto& [workLeft, workTop, workWidth, workHeight] = monitor.WorkArea;
				const auto& [areaLeft, areaTop, areaWidth, areaHeight] = monitor.Area;
				Debug(std::format("  Name: {}", monitor.Name));
				Debug(std::format("    WorkArea: left: {}, top: {}, width: {}, height: {}", workLeft, workTop, workWidth, workHeight));
				Debug(std::format("    Area: left: {}, top: {}, width: {}, height: {}", areaLeft, areaTop, areaWidth, areaHeight));
				Debug(std::format("    Is Primary: {}", monitor.IsPrimary));
			}*/
		}

		return m_AvailableMonitors;
	}

	void MonitorProviderCache::Clear()
	{
		m_PrimaryMonitor = std::nullopt;
		m_AvailableMonitors.clear();
	}

	void MonitorProviderCache::Reset(std::shared_ptr<MonitorProvider> provider)
	{
		m_MonitorProvider = std::move(provider);
		Clear();
	}
}
