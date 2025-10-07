project("DirectGL-LibraryLogger")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build-int/bin/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/**.ixx",
		"private/**.cpp",
	})

	links({
		"Logging"
	})

	filter("system:windows")
		systemversion("latest")

	filter("configurations:Debug")
		runtime("Debug")
		symbols("on")

	filter("configurations:Release")
		runtime("Release")
		optimize("on")