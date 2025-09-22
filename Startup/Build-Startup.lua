project("Startup")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"private/Startup-AppStartup.cpp",

		"public/Startup.ixx",
		"public/Startup-AppStartup.ixx",
		"public/Startup-StartupTask.ixx",
	})

	filter("system:windows")
		systemversion("latest")
		

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")