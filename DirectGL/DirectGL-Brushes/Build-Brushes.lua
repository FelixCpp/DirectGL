project("DirectGL-Brushes")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/**.ixx",
		"internal/**.ixx",
		"private/**.cpp",
	})

	includedirs({
		"%{wks.location}/Libraries/Glad/include",
	})

	links({
		"DirectGL-Renderer",
		"DirectGL-Math",
		"DirectGL-Logging",

		"Preconditions",
		"Glad",
	})

	filter("system:windows")
		systemversion("latest")

	filter("configurations:Debug")
		runtime("Debug")
		symbols("on")

	filter("configurations:Release")
		runtime("Release")
		optimize("on")