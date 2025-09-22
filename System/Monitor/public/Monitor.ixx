export module System.Monitor;

export import :Monitor;
export import :MonitorProvider;
export import :MonitorProviderCache;

#ifdef PLATFORM_WINDOWS
export import :Win32MonitorProvider;
#endif