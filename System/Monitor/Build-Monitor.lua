project("Monitor")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		-- Public
		"public/Monitor.ixx",
		"public/Monitor-Monitor.ixx",
		"public/Monitor-MonitorProvider.ixx",
		"public/Monitor-MonitorProviderCache.ixx",

		-- Private
		"private/Monitor-MonitorProviderCache.cpp",
	})

	links({
		"DirectGL-Math"
	})

	filter("system:windows")
		systemversion("latest")
		defines({ "PLATFORM_WINDOWS" })
		files({
			"public/Win32/Monitor-Win32MonitorProvider.ixx",
			"private/Win32/Monitor-Win32MonitorProvider.cpp",
		})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")