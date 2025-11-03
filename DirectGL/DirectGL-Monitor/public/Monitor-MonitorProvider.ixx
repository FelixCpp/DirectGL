module;

#include <optional>
#include <vector>
#include <memory>

export module DirectGL.Monitor:MonitorProvider;

import :Monitor;

export namespace DGL
{
	struct MonitorProvider
	{
		virtual ~MonitorProvider() = default;
		[[nodiscard]] virtual std::optional<Monitor> GetPrimaryMonitor() const = 0;
		[[nodiscard]] virtual std::vector<Monitor> GetAvailableMonitors() const = 0;
	};

	[[nodiscard]] std::unique_ptr<MonitorProvider> CreateDefaultMonitorProvider();
}