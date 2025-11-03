module;

#include <memory>

module DirectGL.Monitor;

#ifdef PLATFORM_WINDOWS
	import :Win32MonitorProvider;
	typedef DGL::Internal::Win32MonitorProvider MonitorProviderImpl;
#else
	#error Could not find a default MonitorProvider implementation for this platform.
#endif

namespace DGL
{
	std::unique_ptr<MonitorProvider> CreateDefaultMonitorProvider()
	{
		return std::make_unique<MonitorProviderImpl>();
	}
}