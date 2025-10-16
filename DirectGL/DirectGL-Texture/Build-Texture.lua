project("DirectGL-Texture")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")
	
	files({
		"private/**.cpp",
		"public/**.ixx",
	})

	links({
		"DirectGL-Math",

		"Glad",
		"Preconditions",
	})

	includedirs({
		"%{wks.location}/Libraries/Glad/include"
	})

	filter("system:windows")
		systemversion("latest")

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")