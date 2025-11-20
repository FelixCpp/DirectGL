project("Libtess2")
	kind("StaticLib")
	language("C")
	cdialect("C11")
	location("%{wks.location}/Libraries/Libtess2")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	includedirs({
		"%{prj.location}/Include",
	})

	files({
		"%{prj.location}/Include/**.h",
		"%{prj.location}/Source/**.c",
	})

	filter("system:windows")
		systemversion("latest")

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")