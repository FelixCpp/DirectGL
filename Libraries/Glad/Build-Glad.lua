project("Glad")
	kind("StaticLib")
	language("C")
	cdialect("C11")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")
	files({
		"include/**.h",
		"src/**.c"
	})

	includedirs({
		"include"
	})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")