module;

#include <optional>
#include <Windows.h>
#include <bit>
#include <vector>

module System.Monitor;

namespace System
{
	constexpr DGL::Math::IntBoundary RectToBoundary(const RECT& rect)
	{
		return DGL::Math::IntBoundary::FromLTRB(
			static_cast<int32_t>(rect.left),
			static_cast<int32_t>(rect.top),
			static_cast<int32_t>(rect.right),
			static_cast<int32_t>(rect.bottom)
		);
	}

	std::optional<Monitor> Win32MonitorProvider::GetPrimaryMonitor() const
	{
		const HWND desktopWindow = GetDesktopWindow();
		if (desktopWindow == nullptr)
		{
			return std::nullopt;
		}

		const HMONITOR primaryMonitor = MonitorFromWindow(desktopWindow, MONITOR_DEFAULTTOPRIMARY);
		if (primaryMonitor == nullptr)
		{
			return std::nullopt;
		}

		MONITORINFOEXA monitorInfo = {};
		monitorInfo.cbSize = sizeof(MONITORINFOEXA);
		if (not GetMonitorInfoA(primaryMonitor, &monitorInfo))
		{
			return std::nullopt;
		}

		return Monitor{
			.Name = monitorInfo.szDevice,
			.WorkArea = RectToBoundary(monitorInfo.rcWork),
			.Area = RectToBoundary(monitorInfo.rcMonitor),
			.IsPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0
		};
	}

	std::vector<Monitor> Win32MonitorProvider::GetAvailableMonitors() const
	{
		std::vector<Monitor> result;

		const auto insertion = [&result](Monitor&& monitor)
		{
			result.emplace_back(monitor);
		};

		typedef decltype(insertion) InsertionFunction;

		EnumDisplayMonitors(
			nullptr,
			nullptr,
			[](const HMONITOR hMonitor, HDC, LPRECT, const LPARAM dwData) -> BOOL
			{
				MONITORINFOEXA monitorInfo = {};
				monitorInfo.cbSize = sizeof(MONITORINFOEXA);
				if (not GetMonitorInfoA(hMonitor, &monitorInfo))
				{
					return TRUE; // Continue enumeration
				}

				const auto& insert = *std::bit_cast<InsertionFunction*>(dwData);

				insert(Monitor{
					.Name = monitorInfo.szDevice,
					.WorkArea = RectToBoundary(monitorInfo.rcWork),
					.Area = RectToBoundary(monitorInfo.rcMonitor),
					.IsPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0
				});

				return TRUE; // Continue enumeration
			},
			std::bit_cast<LPARAM>(&insertion)
		);

		return result;
	}
}
