project("DirectGL-DPI")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/DPI.ixx",
	})
	
	links({
		"DirectGL-Logging",
	})

	filter("system:windows")
		systemversion("latest")
		defines({ "DPI_PLATFORM_WINDOWS" })
		files({
			"private/Win32/DPIImplWin32.cpp",
		})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")