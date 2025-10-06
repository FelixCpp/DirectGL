project("Preconditions")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")
	
	files({
		"public/Preconditions.ixx",
		"public/Preconditions-Preconditions.ixx",
		"public/Preconditions-Standard.ixx",
	})
	
	filter("configurations:Debug")
		runtime("Debug")
		symbols("on")

	filter("configurations:Release")
		runtime("Release")
		optimize("on")
