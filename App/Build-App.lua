project("App")
	kind("ConsoleApp")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"Main.cpp",
	})

	links({
		"DirectGL-Core",
	})

	filter("system:windows")
		systemversion("latest")

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")