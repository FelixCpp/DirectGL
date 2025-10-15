project("DirectGL-TextureRenderer")
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
		"%{wks.location}/Libraries/Stb/include"
	})

	links({
		"DirectGL-Renderer",
		"DirectGL-Math",

		"Glad",
		"Stb",
	})

	filter("system:windows")
		systemversion "latest"

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")