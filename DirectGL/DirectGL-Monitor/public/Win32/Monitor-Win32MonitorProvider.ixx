export module DirectGL.Monitor:Win32MonitorProvider;

import :MonitorProvider;

namespace DGL::Internal
{
	export struct Win32MonitorProvider : MonitorProvider
	{
		[[nodiscard]] std::optional<Monitor> GetPrimaryMonitor() const override;
		[[nodiscard]] std::vector<Monitor> GetAvailableMonitors() const override;
	};
}