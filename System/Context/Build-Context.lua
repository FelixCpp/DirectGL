project("Context")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/Context.ixx",
		"public/Context-Context.ixx",
		"private/Context-Context.cpp",
	})

	includedirs({
		"%{wks.location}/Libraries/Glad/include"
	})

	links({
		"DirectGL-Math",

		"Glad",
		"Window",
	})

	filter("system:windows")
		systemversion("latest")
		defines({ "CONTEXT_PLATFORM_WINDOWS" })
		files({
			"public/Win32/Context-WGLContext.ixx",
			"private/Win32/Context-WGLContext.cpp",
		})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")