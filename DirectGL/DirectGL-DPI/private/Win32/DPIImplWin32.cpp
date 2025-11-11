module;

#include <Windows.h>

#include <memory>
#include <string_view>

module DirectGL.DPI;

import DirectGL.Logging;

namespace
{
	using LibraryPtr = std::unique_ptr<HINSTANCE__, decltype(&FreeLibrary)>;

	LibraryPtr Load(const std::string_view libraryName)
	{
		return LibraryPtr(LoadLibraryA(libraryName.data()), &FreeLibrary);
	}

	bool EnableShCore()
	{
		const LibraryPtr shCore = Load("shcore.dll");
		if (shCore == nullptr)
		{
			DGL::Logging::Error("Couldn't load shcore.dll");
			return false;
		}

		enum PROCESS_DPI_AWARENESS
		{
			PROCESS_DPI_UNAWARE = 0,
			PROCESS_SYSTEM_DPI_AWARE = 1,
			PROCESS_PER_MONITOR_DPI_AWARE = 2
		};

		using SetProcessDpiAwarenessFunc = HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS);
		const auto functionPtr = reinterpret_cast<SetProcessDpiAwarenessFunc>(GetProcAddress(shCore.get(), "SetProcessDpiAwareness"));
		if (functionPtr == nullptr)
		{
			DGL::Logging::Error("Couldn't get address of SetProcessDpiAwareness");
			return false;
		}

		const HRESULT hr = functionPtr(PROCESS_PER_MONITOR_DPI_AWARE);
		return hr != E_INVALIDARG;
	}

	bool EnableUser32()
	{
		const LibraryPtr user32 = Load("user32.dll");
		if (user32 == nullptr)
		{
			DGL::Logging::Error("Couldn't load user32.dll");
			return false;
		}

		using SetProcessDPIAwareFunc = BOOL(WINAPI*)();
		const auto functionPtr = reinterpret_cast<SetProcessDPIAwareFunc>(GetProcAddress(user32.get(), "SetProcessDPIAware"));
		if (functionPtr == nullptr)
		{
			DGL::Logging::Error("Couldn't get address of SetProcessDPIAware");
			return false;
		}

		return functionPtr() == TRUE;
	}
}

namespace DGL
{
	bool EnableDPIAwareness()
	{
		if (EnableShCore())
		{
			Logging::Info("DPI Awareness set to Per-Monitor using shcore.dll");
			return true;
		}

		if (EnableUser32())
		{
			Logging::Info("DPI Awareness set to System using user32.dll");
			return true;
		}

		return false;
	}
}