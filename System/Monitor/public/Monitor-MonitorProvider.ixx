module;

#include <optional>
#include <vector>

export module System.Monitor:MonitorProvider;

import :Monitor;

export namespace System
{
	struct MonitorProvider
	{
		virtual ~MonitorProvider() = default;
		[[nodiscard]] virtual std::optional<Monitor> GetPrimaryMonitor() const = 0;
		[[nodiscard]] virtual std::vector<Monitor> GetAvailableMonitors() const = 0;
	};
}