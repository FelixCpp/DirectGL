project("Math")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/Math.ixx",
		"public/Math-Boundary.ixx",
		"public/Math-Matrix4x4.ixx",
		"public/Math-Value2.ixx",
		"public/Math-Value4.ixx",
	})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")